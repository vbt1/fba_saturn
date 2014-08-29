/*
	PSX SYSTEM CALL
*/

#ifndef	_SYSCALL_H
#define	_SYSCALL_H

/* typedef */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;

//typedef	unsigned int size_t;

/* define */
/*#ifndef	NULL
#define	NULL	((void*)0)
#endif

#ifndef	TRUE
#define	TRUE	1
#define	FALSE	0
#endif

#define	O_RDONLY	1
#define	O_WRONLY	2
#define	O_RDWR		3
#define	O_CREAT		0x200

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
*/
/* struct */
/*
struct DIRENTRY {
	char name[20];
	long attr;
	long size;
	struct DIRENTRY *next;
	long head;
	char system[4];
};

struct EXEC {
	unsigned long pc0;
	unsigned long gp0;
	unsigned long t_addr;
	unsigned long t_size;
	unsigned long d_addr;
	unsigned long d_size;
	unsigned long b_addr;
	unsigned long b_size;
	unsigned long s_addr;
	unsigned long s_size;
	unsigned long sp,fp,gp,ret,base;
};
*/
/*
typedef int jmp_buf[12];

struct device_buf {
	
};
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
