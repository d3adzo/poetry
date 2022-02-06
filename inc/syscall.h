#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

extern void *sys_call_table[];
static int uid = 0;

asmlinkage int (*original_call)(const char *, int, int);
asmlinkage int (*getuid_call)();
asmlinkage int our_sys_open(const char *, int, int);
