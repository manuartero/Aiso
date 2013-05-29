#ifndef DRIVER_H
#define DRIVER_H

#include <linux/module.h>	/* modulo */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_to_user, copy_from_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev */

#define NOMBRE_DEV "aiso_device"
#define discosize (size_t) (8*PAGE_SIZE)

/* Funciones file_operations */
static int aiso_open(struct inode *inode, struct file *file);
static int aiso_release(struct inode *inode, struct file *file);
static ssize_t aiso_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);
loff_t aiso_llseek(struct file *file, loff_t ppos, int modo);

/* Funciones init exit */
static int __init my_init(void);
static void __exit my_exit(void);


#endif /* DRIVER_H */

