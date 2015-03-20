#include "mysyscall.h"
#include "sys/stat.h"
#include <dirent.h>
/*
void sys_restart_syscall(int callNum);
void sys_exit(int callNum, int errorCode);
void sys_read(int callNum, int desc, char* buf, int len);
void sys_open(int callNum, const char* file, int flag, int mode);
void sys_write(int callNum, int desc, char* buf, int len);
void sys_close(int callNum, int desc);
void sys_unlink(int callNum, int file);
void sys_access(const char* path, int mode);
void my_getpid(int callNum);
void sys_getdents(int desc, struct dirent *dirp, int count);
*/
void sys_restart_syscall(int callNum) {
        MY_SYSCALL0(callNum);
}

void sys_exit(int callNum, int errorCode) {
        MY_SYSCALL1(callNum, errorCode);
}

void sys_read(int callNum, int desc, char* buf, int len) {
        MY_SYSCALL3(callNum, desc, buf, len);
}

void sys_open(int callNum, const char* file, int flag, int mode) {
        MY_SYSCALL4(callNum, file, flag, mode);
}
void sys_write(int callNum, int desc, char* buf, int len) {
        MY_SYSCALL5(callNum, desc, buf, len);
}
void sys_close(int callNum, int desc) {
        MY_SYSCALL6(callNum, desc);
}

void sys_unlink(int callNum, int file) {
        MY_SYSCALL10(callNum, file);
}

void my_getpid(int callNum) {
        MY_SYSCALL20(callNum);
}

void sys_access(const char* path, int mode) {
        MY_SYSCALL33(33, path, mode);
}

void sys_getdents(int desc, struct dirent *dirp, int count) {
        MY_SYSCALL141(141, desc, dirp, count);
}


