#ifndef DRIVER_H
#define DRIVER_H

#include <linux/module.h>	    /* modulo */
#include <linux/fs.h>		    /* file_operations */
#include <linux/uaccess.h>	    /* copy_to_user, copy_from_user */
#include <linux/init.h>		    /* module_init, module_exit */
#include <linux/slab.h>		    /* kmalloc */
#include <linux/vmalloc.h>      /* vmalloc */
#include <linux/cdev.h>		    /* cdev */
#include <linux/kthread.h>      /* kthread */
#include "./funciones_ioctl.h"  /* funciones ioctl */

#define NOMBRE_THREAD "driver_thread"
#define discosize (size_t) (8*PAGE_SIZE)

/* Funciones file_operations */
static int aiso_open(struct inode *inode, struct file *file);
static int aiso_release(struct inode *inode, struct file *file);
static ssize_t aiso_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);

/* Funciones IOCTL */
extern int aiso_ioctl
(struct inode * inode, struct file * file, unsigned int ioctl_num, unsigned long ioctl_param);
static loff_t aiso_lseek(struct file * file, loff_t ppos, int modo);
static void aiso_reset(struct file * fichero);

/* Funciones init exit */
static int __init aiso_init(void);
static void __exit aiso_exit(void);

/* Funcion del kthread asociado */
int funcion_thread(void * data);


#endif /* DRIVER_H */

