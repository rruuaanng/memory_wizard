#ifndef _MOON_PROC_MEM_WIZARD_H
#define _MOON_PROC_MEM_WIZARD_H

/*==========================
	ioctl command
============================*/
#define GET_VERSION 				_IOR('W', 0x0001, char*)
#define GET_PROC_ID					_IOR('W', 0x0002, pid_t)
// #define GET_PROCESS				

#endif // _MOON_PROC_MEM_WIZARD_H