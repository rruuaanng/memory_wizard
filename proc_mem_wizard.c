#include <linux/sched/task.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>

#include "proc_mem_wizard.h"

/*==========================
global struct
============================*/
struct proc_desc {
	pid_t pid;
	char *name;
	__u64 entry_addr;
	__u64 end_addr;
	struct proc_desc *parent;
}target_proc;

/*==========================
wizard infomation
============================*/
#define WIZARD_VERSION 				"v0.0.1"
#define MODULE_NAME 				"proc_mem_wizard"	

/*==========================
user interaction implement
============================*/
ssize_t read_from_user_pg(struct file *fp, char __user *buf,
						  size_t, loff_t *)
{
	return 0;
}
ssize_t write_to_user_pg(struct file *fp, const char __user *buf,
						 size_t, loff_t *)
{
	return 0;
}

int user_open(struct inode *node, struct file *fp)
{
	return 0;
}

int user_release(struct inode *node, struct file *fp)
{
	return 0;
}

long user_ioctl_m64(struct file *fp, unsigned int cmd, unsigned long arg) {
	// check userspace point not null
	if ((void *)arg == NULL) {
		return -EFAULT;
	}

	// select command and execute
	switch (cmd) {
	case GET_VERSION:
		// user point address
		char __user *ubuf = (char __user*)arg;

		// copy to user code
		if(copy_to_user(ubuf,WIZARD_VERSION,sizeof(WIZARD_VERSION))) {
			pr_err("version info copy to userspace error\n");
		}
		break;
	
	default:
		pr_err("ioctl command error\n");
		break;
	}

	return 0;
}

long user_ioctl_m32(struct file *fp, unsigned int cmd, unsigned long arg) {
	// check userspace point not null
	if ((void *)arg == NULL) {
		return -EFAULT;
	}

	// select command and execute
	switch (cmd) {
	case GET_VERSION:
		// user point address
		char __user *ubuf = (char __user*)arg;

		// copy to user code
		if(copy_to_user(ubuf,WIZARD_VERSION,sizeof(WIZARD_VERSION))) {
			pr_err("version info copy to userspace error\n");
		}
		break;
	
	default:
		pr_err("ioctl command error\n");
		break;
	}

	return 0;
}

/*==========================
core function
============================*/
pid_t scan_proc_pid(const char *name)
{
	struct task_struct *p;

 	// from init_task.tasks search proc
	list_for_each_entry(p, &init_task.tasks, tasks) {
		// cmp proc name
		if (strcmp(name,p->comm)){
			pr_info("find proc : pid{%d} name{%s}\n", 
				   p->pid, p->comm);

			return p->pid;
		}
	}

	return 0;
}

/*==========================
global variable
============================*/
struct cdev wizard_cdev;
struct class *wizard_cls;
struct device *wizard_dev;
dev_t wizard_id;
struct file_operations user_ops_impl = {
	.owner = THIS_MODULE,
	.compat_ioctl = user_ioctl_m64,
	.unlocked_ioctl = user_ioctl_m32,
	.release = user_release,
	.open = user_open
};

/*==========================
initilize proc memory wizard 
============================*/
int __init wizard_init(void)
{
	// alloc cdev id
	if (alloc_chrdev_region(&wizard_id,0,1,MODULE_NAME) < 0) {
		pr_err("device id alloc failed!\n");
	}

	// init cdev
	cdev_init(&wizard_cdev,&user_ops_impl);
	wizard_cdev.owner = THIS_MODULE;
	wizard_cdev.ops = &user_ops_impl;
	
	// reigster cdev to kernel
	if (cdev_add(&wizard_cdev,wizard_id,1) < 0) {
		pr_err("user interface create failed!\n");
		goto cdev_add_err;
	}

	// init dev class
	wizard_cls = class_create(THIS_MODULE,MODULE_NAME);
	if (IS_ERR(wizard_cls)) {
		pr_err("device class alloc failed!\n");
		goto cls_create_err;
	}

	// init sysfs device
	wizard_dev = device_create(wizard_cls,NULL,wizard_id,NULL,MODULE_NAME);
	if (IS_ERR(wizard_dev)) {
		pr_err("device create failed!\n");
		goto dev_create_err;
	}
	
	pr_info("proc memory wizard has started!\n");
	return 0;

// error
dev_create_err:
	class_destroy(wizard_cls);
cls_create_err:
	cdev_del(&wizard_cdev);
cdev_add_err:
	unregister_chrdev_region(wizard_id,1);

	pr_err("proc memory wizard start failed!\n");
	return -ENODEV;
}

// exit this module
void __exit wizard_exit(void)
{
	cdev_del(&wizard_cdev);
	device_unregister(wizard_dev);
	device_destroy(wizard_cls,wizard_id);
	unregister_chrdev_region(wizard_id,1);
	class_destroy(wizard_cls);
	pr_info("proc memory wizard has exited\n");
}

module_init(wizard_init);
module_exit(wizard_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ruang");
MODULE_DESCRIPTION("proc memory wizard, easy for users to modify proc memory");