// SPDX-License-Identifier: GPL-2.0
/*
 * ksu_exports.c: re-export kernel symbols required by the KernelSU module
 * packaged as kernelsu-dkms (supechicken/KernelSU, waydroid-experimental).
 *
 * That module is written against Android GKI-style vendor export lists and
 * references internal symbols that upstream kernels keep unexported, so on
 * GXDE kernels it previously could only be loaded through a userspace
 * loader that relocated the symbols from /proc/kallsyms. Exporting them
 * here makes the module loadable directly with modprobe.
 *
 * Exports are limited to x86_64 (not i386) and arm64 — the architectures
 * the KernelSU module is shipped for in this distribution. Other
 * architectures are deliberately excluded: some of these symbols may not
 * even exist there (e.g. ext4_unregister_sysfs when ext4 is built as a
 * module), and those targets still load the module through the userspace
 * loader instead.
 *
 * The exported set matches the module's undefined symbols for this kernel
 * version; signatures must be kept in sync with this tree.
 *
 * Differences to newer branches: this tree still has the three-argument
 * change_pid() (the four-argument version landed in mainline 6.15, together
 * with free_pids()) and predates dentry_open_nonotify() (mainline 6.14),
 * so the module built for it uses the corresponding code paths and does
 * not reference free_pids / dentry_open_nonotify.
 */

#include <linux/module.h>

#if defined(__x86_64__) || defined(__aarch64__)

#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/seccomp.h>
#include <linux/uaccess.h>
#include <linux/pid.h>
#include <linux/path.h>
#include <linux/fs_struct.h>
#include <linux/proc_ns.h>
#include <linux/tracepoint.h>
#include <linux/spinlock_types.h>
#include <asm/ptrace.h>
#include "../fs/internal.h"		/* path_mount(), path_umount() */

struct super_block;
struct file;
struct mm_struct;
struct user_struct;

/* ext4 sysfs teardown has no module-visible prototype */
void ext4_unregister_sysfs(struct super_block *sb);

/* pid namespace internals (signatures vary between kernel branches) */
void change_pid(struct task_struct *task, enum pid_type type, struct pid *pid);

/* data symbols without declarations in this tree's public headers */
extern struct tracepoint __tracepoint_sched_process_exec;
extern struct tracepoint __tracepoint_sched_process_exit;
extern struct tracepoint __tracepoint_sys_enter;
extern struct mm_struct init_mm;
extern rwlock_t tasklist_lock;

/* Syscall wrappers are per-architecture and have no header prototypes. */
#if defined(__x86_64__)
asmlinkage long __x64_sys_close(const struct pt_regs *regs);
asmlinkage long __x64_sys_unshare(const struct pt_regs *regs);
asmlinkage long __x64_sys_setns(const struct pt_regs *regs);
EXPORT_SYMBOL_GPL(__x64_sys_close);
EXPORT_SYMBOL_GPL(__x64_sys_unshare);
EXPORT_SYMBOL_GPL(__x64_sys_setns);
#else /* __aarch64__ */
asmlinkage long __arm64_sys_close(const struct pt_regs *regs);
asmlinkage long __arm64_sys_unshare(const struct pt_regs *regs);
asmlinkage long __arm64_sys_setns(const struct pt_regs *regs);
EXPORT_SYMBOL_GPL(__arm64_sys_close);
EXPORT_SYMBOL_GPL(__arm64_sys_unshare);
EXPORT_SYMBOL_GPL(__arm64_sys_setns);
#endif

EXPORT_SYMBOL_GPL(kallsyms_lookup_name);
EXPORT_SYMBOL_GPL(kallsyms_lookup);
EXPORT_SYMBOL_GPL(kallsyms_lookup_size_offset);
EXPORT_SYMBOL_GPL(kallsyms_on_each_symbol);
EXPORT_SYMBOL_GPL(path_mount);
EXPORT_SYMBOL_GPL(path_umount);
EXPORT_SYMBOL_GPL(copy_to_kernel_nofault);
EXPORT_SYMBOL_GPL(strncpy_from_user_nofault);
EXPORT_SYMBOL_GPL(ext4_unregister_sysfs);
EXPORT_SYMBOL_GPL(find_task_by_vpid);
EXPORT_SYMBOL_GPL(set_fs_pwd);
EXPORT_SYMBOL_GPL(ns_get_path);
EXPORT_SYMBOL_GPL(mntns_operations);
EXPORT_SYMBOL_GPL(init_mm);
EXPORT_SYMBOL_GPL(tasklist_lock);
EXPORT_SYMBOL_GPL(change_pid);
EXPORT_SYMBOL_GPL(alloc_uid);
EXPORT_SYMBOL_GPL(set_cred_ucounts);
EXPORT_SYMBOL_GPL(get_mm_exe_file);
EXPORT_SYMBOL_GPL(seccomp_filter_release);
EXPORT_SYMBOL_GPL(__tracepoint_sched_process_exec);
EXPORT_SYMBOL_GPL(__tracepoint_sched_process_exit);
EXPORT_SYMBOL_GPL(__tracepoint_sys_enter);

#endif /* __x86_64__ || __aarch64__ */
