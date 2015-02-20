#include "mysyscall.h"
#include "mysyscall.c"
#include "myhelpers.h"
#include "myhelpers.c"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define O_RDWR 02
#define O_CREAT 0100
#define MAX_LINE_BUFFER 1024 //only will read lines of 1024 byte length..seems long enough

typedef enum {false, true} bool;

void util_start(void);

asm (".global util_start\r\n"
     "  .type util_start,@function\r\n"
     ".global main\r\n"
     "  .type main,@function\r\n");

/* Get the argc and argv values in the right registers */
asm ("util_start:\r\n"
     "movl %esp, %eax\r\n"
     "addl $4, %eax\r\n"
     "pushl %eax\r\n"
     "subl $4, %eax\r\n"
     "pushl (%eax)\r\n"
     "  call main\r\n");

int main(int argc, char **argv) {
  int return_code = argc;
  
  char* helpString = "Usage: mygrep [-chrv] pattern file|directory|- outfile|-\n"\
  "-c NUM: Stop after NUM matches (or when out of input). The default is to search all input\n"\
  "-h: Print help message and exit\n"\
  "-r: Search recursively into the children if the search target is a directory.\n"\
  "-v: Reverse the pattern, i.e., print lines that do not match the pattern.\n";
  // Exercise 6: Your code here.
  //need to catch op codes during program execution
  //we cant use getops due to lack of stdlib
  //process every element of argc, on "-" if length > 1 then traverse that char * toggle op codes for each case
  //assuming the operation should FAIL if there is a bad opcode, we will have to sys_exit EXIT_FAILURE for bad codes
  //lets do it in a for loop
  int opIndex, opIndex2 = 0;
  int inDesc, outDesc;
  int cNum = 0;
  bool cB = false, rB = false, vB = false; //these are flags for what op codes user input
  bool opComplete = false, inputComplete = false, patternComplete = false, cNumFlag = false; //flags for program logic
  bool sinFlag = false; bool soutFlag = false; //flags for std_in and std_out usage.
  char inputPattern[100];
  char inputFile[64];
  char outputFile[64]; //char* for functionality
  //for loop should process every element, inside the for loop we disect the elements
  for(opIndex = 1; opIndex < argc; opIndex++) {
    // && argv[opIndex][0] == '-';
    // //if we found a - and its length is less than 2, it's not a valid op code && we
    // if(strlen(argv[opIndex][opIndex2] < 2)) { sys_exit(1, EXIT_FAILURE); }
    // else { }
    if(cB == true && cNumFlag == false) { cNum = my_atof(argv[opIndex]); cNumFlag = true; } //if cB flag was set, nextarg must be the NUMBER
    else if(argv[opIndex][0]  == '-') {
      if(opComplete == false) { //if the first char is a - and we havent received the pattern yet
                        //check this char* for op codes using another for loop
        for(opIndex2 = 1; opIndex2 < strlen(argv[opIndex]); opIndex2++) {
          switch(argv[opIndex][opIndex2]) {
            case 'c': cB = true; break;
            case 'h': sys_write(4, STD_OUT, helpString, strlen(helpString)); sys_exit(1, EXIT_SUCCESS);
            case 'r': rB = true; break;
            case 'v': vB = true; break;
            default: sys_write(4, STD_OUT, helpString, strlen(helpString)); sys_exit(1, EXIT_FAILURE);
          }//switch
        }//for
      }//if opCompelte 
      else { //opCodes were already found, check if input was found, then output
        opComplete = true;
        if(inputComplete == false) {
          //assign input to std out
          sinFlag = true;
          inputComplete = true;
        } 
        else { /*assign output to std out */ 
          soutFlag = true;
          break;
        }
      }
    } //if
    else { //if it is not a - then we need to confirm we received op codes, and find out where we are;
      opComplete = true;
      if(opComplete == true) { //set opComplete to true
        if(!patternComplete) { /*This char* is the pattern! */
          my_strcpy(argv[opIndex], inputPattern);
          //inputPattern = argv[opIndex];
          if(inputPattern != NULL) { patternComplete = true; } else { sys_exit(1, EXIT_FAILURE); }
         }
        else if(!inputComplete) { /*This char* is the input file */
          //inputFile = argv[opIndex];
          my_strcpy(argv[opIndex], inputFile);
          if(inputFile != NULL) { inputComplete = true; } else { sys_exit(1, EXIT_FAILURE); }
        }
        else { /*This char* is the output file */
          //outputFile = argv[opIndex];
          my_strcpy(argv[opIndex], outputFile);
          if(outputFile != NULL) { break; } else { sys_exit(1, EXIT_FAILURE); }
        }
      }
    } //if/else
  }//for loop - get opt replacement

  //Okay we got our OP codes, lets error check what we got
  if(cB){ sys_write(4, STD_OUT, "CB\n", 3); }
  if(rB){ sys_write(4, STD_OUT, "rB\n", 3); }
  if(vB){ sys_write(4, STD_OUT, "vB\n", 3); }
  sys_write(4, STD_OUT, inputPattern, strlen(inputPattern));
  sys_write(4, STD_OUT, "\n", 1);
  sys_write(4, STD_OUT, inputFile, strlen(inputFile));
  sys_write(4, STD_OUT, "\n", 1);
  sys_write(4, STD_OUT, outputFile, strlen(outputFile));
  sys_write(4, STD_OUT, "\n", 1);
  if(cNum > 2) { sys_write(4, STD_IN, "woo\n", 4); }

  //Okay "GETOPTS" works, time to confirm files exist, store descriptors etc.
  if(sinFlag == true) { // input will be handled from std_in
    inDesc = STD_IN;
  } else { //lets get the file descriptor and store it
    inDesc = MY_SYSCALL4(5, inputFile, O_CREAT|O_RDWR, 00700);
    if(inDesc < 0) { sys_exit(1, EXIT_FAILURE); } //no file desc.
  }
  if(soutFlag == true) { // input will be handled from std_in
    outDesc = STD_OUT;
  } else { //lets get the file descriptor and store it
    outDesc = MY_SYSCALL4(5, outputFile, O_CREAT|O_RDWR, 00700);
    if(outDesc < 0) { sys_write(4, STD_OUT, "flt\n", 4); sys_exit(1, EXIT_FAILURE); } //no file desc.
  }
  sys_write(4, outDesc, inputPattern, strlen(inputPattern));
  sys_close(5, inDesc);
  sys_close(6, outDesc);
  //can open/close files we have the descriptors, lets start writing/searching.

  // Exit.  Until this is implemented,
  // your program will hang or give a segmentation fault.
  MY_SYSCALL1(1, return_code);
  return return_code;
}




