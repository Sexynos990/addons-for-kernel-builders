#include <linux/module.h>
#include <linux/kprobes.h>

static struct kprobe kernelsu_detect_kp = {
    .symbol_name = "security_bprm_check",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    struct linux_binprm *bprm = (struct linux_binprm *)regs->di;
    
    // Hide KernelSU binaries from security checks
    if (strstr(bprm->filename, "kernelsu")) {
        return -ENOENT; // Fake "file not found"
    }
    
    return 0;
}

static void __init install_antidetect(void)
{
    kernelsu_detect_kp.pre_handler = handler_pre;
    register_kprobe(&kernelsu_detect_kp);
}
