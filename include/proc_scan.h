#ifndef _MOON_PROC_SCAN_H
#define _MOON_PROC_SCAN_H


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


#endif // _MOON_PROC_SCAN_H