/* Driver de caracteres sencillo */

#include <linux/module.h>	/* modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev */

#define NOMBRE_DEV "aisodrv"

static char *discoram;
#define discosize (size_t) (8*PAGE_SIZE)

static dev_t first; // acumula mayor y menor
static unsigned int count = 1; // vamos a registrar 1
static int my_major = 700, my_minor = 0;
static struct cdev *my_cdev; // la estructura del driver

static int aiso_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO " Abriendo dispositivo: %s:\n\n", NOMBRE_DEV);
	return 0;
}

static int aiso_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO " Cerrando dispositivo: %s:\n\n", NOMBRE_DEV);
	return 0;
}

static ssize_t
aiso_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = lbuf - copy_to_user(buf, discoram + *ppos, lbuf);
	*ppos += nbytes;
	printk(KERN_INFO "\n callback lectura, nbytes=%d, pos=%d\n", nbytes,
	       (int)*ppos);
	return nbytes;
}

static ssize_t
aiso_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int nbytes = lbuf - copy_from_user(discoram + *ppos, buf, lbuf);//q demonios es discoram + *ppos
	*ppos += nbytes;
	printk(KERN_INFO "\n callback escritura, nbytes=%d, pos=%d\n", nbytes,
	       (int)*ppos);
	return nbytes;
}

// las operaciones 
static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
};

static int __init my_init(void)
{
	discoram = kmalloc(discosize, GFP_KERNEL);
	first = MKDEV(my_major, my_minor);
	register_chrdev_region(first, count, NOMBRE_DEV);
	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdrv_fops);
	cdev_add(my_cdev, first, count);
	printk(KERN_INFO "\n Se ha regristrado dispostivo de caracteres %s\n",
	       NOMBRE_DEV);
	return 0;
}

static void __exit my_exit(void)
{
	cdev_del(my_cdev);
	unregister_chrdev_region(first, count);
	printk(KERN_INFO "\nliberado registro de caracteres\n");
	kfree(discoram);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("AISO Lab");
MODULE_LICENSE("GPL v2");
