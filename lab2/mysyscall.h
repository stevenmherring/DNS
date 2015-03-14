#ifndef __MYSYSCALL_H__
#define __MYSYSCALL_H__
#include <dirent.h>
#define O_CREAT     0100
#define O_RDONLY    0000
#define O_RDWR	    0002
#define O_WRONLY    0001
#define ENOSYS      38
#define O_DIRECTORY 0200000

// Exercise 5: Your code here
// Populate each of these functions with appropriate
// assembly code for each number of system call arguments.
//
// Go ahead and fill in all 7 variants, as you will eventually
// need them.
//
// Friendly advice: as you figure out the signature of a system
// call, you might consider writing a macro for it for future reference,
// like:
//
// #define MY_GETPID(...) MY_SYSCALL...(...)


#define MY_SYSCALL0(NUM)		\
  ({							          \
    int rv = -ENOSYS;				\
    asm volatile (          \
      "movl %1, %%eax;"			\
    	"int $0x80;" 					\
    	"movl %%eax, %0;" 		\
    	:"=r" (rv)					  \
    	:"r" (NUM)					  \
    	:"%eax"						    \
    	);						        \
    rv;							        \
  })

#define MY_SYSCALL1(NUM, ARG1)\
  ({							          \
    int rv = -ENOSYS;				\
    asm volatile (          \
      "movl %1, %%eax;"			\
      "movl %2, %%ebx;"     \
      "int $0x80;" 					\
      "movl %%eax, %0;" 		\
      :"=r" (rv)				  	\
      :"r" (NUM), "r" (ARG1)\
      :"%eax"	,"%ebx"				\
      );						        \
    rv;							        \
  })


#define MY_SYSCALL2(NUM, ARG1, ARG2)\
   ({							            \
     int rv = -ENOSYS;				\
     asm volatile (           \
       "movl %1, %%eax;"			\
       "movl %2, %%ebx;"			\
       "movl %3, %%ecx;"			\
       "int $0x80;" 					\
       "movl %%eax, %0;" 			\
       :"=r" (rv)					    \
       :"r" (NUM), "r" (ARG1), "r" (ARG2)\
       :"%eax","%ebx","%ecx"	\
       );						          \
     rv;					           	\
   })


#define MY_SYSCALL3(NUM, ARG1, ARG2, ARG3)\
   ({							              \
     int rv = -ENOSYS;					\
     asm volatile (             \
       "movl %1, %%eax;"			  \
       "movl %2, %%ebx;"			  \
       "movl %3, %%ecx;"			  \
       "movl %4, %%edx;"			  \
       "int $0x80;" 					  \
       "movl %%eax, %0;" 				\
       :"=r" (rv)					      \
       :"g" (NUM), "g" (ARG1), "g" (ARG2), "g" (ARG3)\
       :"%eax","%ebx","%ecx","%edx"\
       );					            	\
     rv;						            \
   })

#define MY_SYSCALL4(NUM, ARG1, ARG2, ARG3, ARG4)	\
   ({							              \
     int rv = -ENOSYS;					\
     asm volatile (             \
       "movl %1, %%eax;"			  \
       "movl %2, %%ebx;"			  \
       "movl %3, %%ecx;"			  \
       "movl %4, %%edx;"			  \
       "movl %5, %%esi;"			  \
       "int $0x80;" 				   	\
       "movl %%eax, %0;" 				\
       :"=r" (rv)					      \
       :"r" (NUM)					      \
       :"%eax"						      \
       );						            \
     rv;					             	\
   })

#define MY_SYSCALL5(NUM, ARG1, ARG2, ARG3, ARG4, ARG5)	\
   ({							              \
     int rv = -ENOSYS;					\
     asm volatile (             \
     "movl %1, %%eax;"			  \
     "movl %2, %%ebx;"			  \
     "movl %3, %%ecx;"			  \
     "movl %4, %%edx;"			  \
     "movl %5, %%esi;"			  \
     "movl %6, %%edi;"			  \
     "int $0x80;" 					  \
     "movl %%eax, %0;" 				\
     :"=r" (rv)					      \
     :"r" (NUM)					      \
     :"%eax"						      \
     );						            \
    rv;							            \
   })


/****************************************************
Definition of the open syscall
Name        #    |  ARG1                          | ARG2       |  ARG3
sys_open	  0x05 | 	const char __user *filename	  | int flags  |	int mode
*****************************************************/
#define MY_OPEN(ARG1, ARG2, ARG3) MY_SYSCALL3(5,ARG1,ARG2,ARG3)


/****************************************************
Definition of the write syscall
Name        #    |   ARG1              |  ARG2                    | ARG3
sys_write	 0x04	 |   unsigned int fd	 |  const char __user *buf	| size_t count
stdin = 0
stdout = 1
stderr = 2
*****************************************************/

#define MY_WRITE(FD, BUFFER, AMOUNT) MY_SYSCALL3(4,FD,BUFFER,AMOUNT)

/****************************************************
Definition of the open syscall
Name        #    |  ARG1
sys_chdir	12     |	const char __user *filename
*****************************************************/

#define MY_CHDIR(filename) MY_SYSCALL1(12,filename)

/****************************************************
Definition of the read syscall
Name        #    |   ARG1             |  ARG2               | ARG3
sys_read	0x03	 |   unsigned int fd	|   char __user *buf	| size_t count	-
*****************************************************/
#define MY_READ(FD, BUFFER, AMOUNT) MY_SYSCALL3(3,FD,BUFFER,AMOUNT)

/****************************************************
Definition of the getdents syscall
Name          #    |   ARG1              |  ARG2                               |  ARG3
sys_getdents	0x8d |   unsigned int fd	 |  struct linux_dirent __user *dirent | 	unsigned int count
*****************************************************/
struct linux_dirent {
        long           d_ino;
        size_t          d_off;
        unsigned short d_reclen;
        char           d_name[];
    };

#define MY_GETDENTS(FD,linux_dirent, COUNT) MY_SYSCALL3(141,FD, linux_dirent, COUNT)

/****************************************************
Definition of the sys_unlink syscall
Name          #    |   ARG1
sys_unlink	 0x0a	 |   const char __user *pathname
*****************************************************/
#define MY_UNLINK(pathname) MY_SYSCALLS1(10,pathname)
// Defing my own syscalls

int inline myGetDentsFunction (int fd,char * dir,size_t count){
  if (count < 0){
    return 0;
  }
  int read= MY_GETDENTS(fd,dir,count);
  return read;
}

void inline myExitFunction (int RV){
  MY_SYSCALL1(1,RV);
}

void inline myWriteFunction (int FD,char *BUFFER,int AMOUNT){
  MY_WRITE(FD,BUFFER,AMOUNT);
}

/*
int inline myGETDENTS (int FD,linux_dirent *dirp,int COUNT){
  MY_GETDENTS(FD,struct linux_dirent *dirp,COUNT);
}
*/

int inline myReadFunction (int FD,char *BUFFER,int AMOUNT){
  int rv = MY_READ(FD,BUFFER,AMOUNT);
  return rv;
}

int inline myOpenFunction (ARG1, ARG2, ARG3){
  int fd = MY_OPEN(ARG1, ARG2 , ARG3);
  return fd;
}

void inline myUnlinkFunction (char* fileName){
  MY_SYSCALL1(10,fileName);
}


void inline myCHDIR (char* fileName){
  MY_CHDIR(fileName);
}
#endif // __MYSYSCALL_H__
