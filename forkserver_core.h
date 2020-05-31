#ifndef __FORKSERVER_CORE_H__
#define __FORKSERVER_CORE_H__

extern int child_pid = 0;

void timer_handler(int pid);
int fail_handler(char *msg);
int exec_testcase(char **argv, int *ret_status);

#endif