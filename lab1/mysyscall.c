#include "mysyscall.h"


void sys_restart_syscall(int callNum);
void sys_exit(int callNum, int errorCode);
void sys_read(int callNum, int desc, char* buf, int len);
void sys_open(int callNum, const char* file, int flag, int mode);
void sys_write(int callNum, int desc, char* buf, int len);
void sys_close(int callNum, int desc);
void my_getpid(int callNum);

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

void my_getpid(int callNum) {
	MY_SYSCALL20(callNum);
}
