#include "driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(my_init);
module_exit(my_exit);

/* Variables globales */
static char *discoram;
static dev_t first; // acumula mayor y menor
static unsigned int count = 1; // vamos a registrar 1
static int my_minor = 0;
static struct cdev *my_cdev; // la estructura del driver
static unsigned int escrito = 0;

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
};

/* Funciones init y exit*/
static int __init my_init(void)
{
	discoram = kmalloc(discosize, GFP_KERNEL);
	alloc_chrdev_region(&first, my_minor, count, NOMBRE_DEV);
	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdrv_fops);
	cdev_add(my_cdev, first, count);
	printk(KERN_INFO "\n Se ha regristrado dispostivo de caracteres %s\n", NOMBRE_DEV);
	return 0;
}

static void __exit my_exit(void)
{
	cdev_del(my_cdev);
	unregister_chrdev_region(first, count);
	printk(KERN_INFO "\nliberado registro de caracteres\n");
	kfree(discoram);
}

/* Funciones file_operations */
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

static ssize_t aiso_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes =0;
	
	if (*ppos >= lbuf)
		return 0;
		
	nbytes = lbuf - copy_to_user(buf, discoram + *ppos, escrito);
	*ppos += nbytes;
    
	printk(KERN_INFO "\n callback lectura, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return escrito;
}

static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = lbuf - copy_from_user(discoram + *ppos, buf, lbuf);//q demonios es discoram + *ppos=es para mover la posicion de memoria a la q apunta el array discoram
	*ppos += nbytes;
	discoram[*ppos] = '\0';
	escrito = nbytes;
	printk(KERN_INFO "\n callback escritura, nbytes=%d, pos=%d\n", nbytes,(int)*ppos);
	return nbytes;
}

