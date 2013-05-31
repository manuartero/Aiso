#include "driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(aiso_init);
module_exit(aiso_exit);

/* Variables globales */
static char *buffer;
static dev_t num_mayor_menor; 
static unsigned int num_dispositivos = 1; 
static int num_menor = 0;
static struct cdev *driver_struct;
static struct task_struct *kthread;

static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
    .ioctl = aiso_ioctl
};

static unsigned int veces_abierto;
int num_mayor = 0;

/* ********************* */
/* Funciones init y exit */
/* ********************* */

static int __init aiso_init(void)
{
    // 1) reservamos memoria para el buffer	
    buffer = kmalloc(discosize, GFP_KERNEL);
    
    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menor, num_menor, num_dispositivos, NOMBRE_DEV);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_struct = cdev_alloc();
	cdev_init(driver_struct, &driver_fops);
	cdev_add(driver_struct, num_mayor_menor, num_dispositivos);
	
    // 4) inicializamos variables
    veces_abierto = 0;  
    num_mayor = MAJOR(num_mayor_menor);

    // 5) creamos el kernel thread asociado
    kthread = kthread_run(funcion_thread, NULL, NOMBRE_THREAD);
    
    printk(KERN_INFO "Dispostivo de caracteres %s y thread asociado %s\n", NOMBRE_DEV, NOMBRE_THREAD);
	return 0;
}

static void __exit aiso_exit(void)
{
    // 1) borramos struct cdev *driver_struct
	cdev_del(driver_struct);
    
    // 2) des-registramos numero mayor y menor 
	unregister_chrdev_region(num_mayor_menor, num_dispositivos);

    // 3) liberamos el buffer	
    kfree(buffer);
	
    // 4) paramos el kthread 
    kthread_stop(kthread);

    printk(KERN_INFO "\nliberado registro de caracteres\n");
    return; 
}


/* ************************* */
/* Funciones file_operations */
/* ************************* */

/**
 * Al abrir:
 *  - sumamos en <veces_abierto>
 *  - despertamos al kernel thread asociado
 */
static int aiso_open(struct inode *inode, struct file *file)
{
    veces_abierto++;	
    wake_up_process(kthread);
    return 0;
}

static int aiso_release(struct inode *inode, struct file *file)
{
	return 0;
}

/** 
 * @param struct file * file: fichero del que leemos
 * @param char * buf: buffer del que leemos
 * @param size_t lbuf: longitud del buffer
 * @param loff_t * ppos: posicion donde empezamos a leer
 */
static ssize_t aiso_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = 0;
    struct cdev_ampliado * dev = file->private_data;

	if (*ppos >= lbuf) {
		return 0;
    }
		
	nbytes = lbuf - copy_to_user(buf, buffer + *ppos, nbytes);
	*ppos += nbytes;
    
    if(copy_to_user(buf, buffer, dev->size) != 0){
        printk(KERN_ALERT "ERROR : aiso_read => copy_to_user\n");
        return -EFAULT;
    }

	printk(KERN_INFO "aiso_read => nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = 0;
    struct cdev_ampliado * dev = file->private_data;

    nbytes = lbuf - copy_from_user(buffer + *ppos, buf, lbuf);	
    *ppos += nbytes;
    
    if (copy_from_user(buffer, buf, lbuf) != 0) {
        printk(KERN_ALERT "ERROR : aiso_write => copy_from_user \n");
        return -EFAULT;
    }	

    buffer[(int)*ppos] = '\0';
    if(dev->size < *ppos){
        dev->size = *ppos;
    }	

	printk(KERN_INFO "aiso_write, nbytes=%d, pos=%d\n", nbytes,(int)*ppos);
	return nbytes;
}


/* *************** */
/* Funciones IOCTL */
/* *************** */

extern int aiso_ioctl
(struct inode * inode, struct file * file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int char_leidos = 0;
    char * ultima_pos = 0;
    char * buffer_entrada = NULL;
    unsigned int long_buffer = 0;

    switch(ioctl_num){
        case IOCTL_READ: 
            char_leidos = aiso_read(file, (char *) ioctl_param, discosize, 0);
            ultima_pos = (char *) ioctl_param + char_leidos;
            put_user('\0', ultima_pos);
        break;

        case IOCTL_WRITE:
            buffer_entrada = (char *) ioctl_param;
            long_buffer = (unsigned int) strlen(buffer_entrada);
            aiso_write(file, (char *) ioctl_param, long_buffer, 0);
        break;

        case IOCTL_LSEEK: 
            aiso_lseek(file, (int) ioctl_param, 0);
        break;

        case IOCTL_RESET: 
            aiso_reset();
        break;

        default: return -ENOTTY;
    }

    return 0;
}

/**
 * http://www.makelinux.net/ldd3/chp-3-sect-5
 *
 * Posiciona el puntero
 *   ppos si modo == 0
 *   actual +  ppos si modo == 1
 *   final si modo == 2
 * @param loff_t ppos: nueva posicion (2 interpretaciones)
 * @param int modo: determina el comportamiento de ppos
 */
static loff_t aiso_lseek(struct file *file, loff_t ppos, int modo)
{	
    struct cdev_ampliado *dev = file->private_data;
    loff_t newppos;

    // calculamos la nueva posicion
    switch(modo) {
        // SEEK_SET  
        case 0: newppos = ppos;
        break;
        
        // SEEK_CUR
        case 1: newppos = file->f_pos + ppos;
        break;
    
        // SEEK_END
        case 2: 
        newppos = dev->size + ppos;
        break;

        // ERROR
        default: return -EINVAL;
    }
    if (newppos < 0) return -EINVAL;
    
    // guardamos la nueva posicion
    file->f_pos = newppos;
    
    printk(KERN_INFO "LSEEK : %d\n", (int)newppos);
    return newppos;
}

static void aiso_reset(void)
{
    // TODO
}


/* **************************** */
/* Funcion del kthread asociado */
/* **************************** */

int funcion_thread(void * data)
{
    allow_signal(SIGKILL);
    
    while(1){
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();

        if(signal_pending(current)){
            break;
        }

        if(kthread_should_stop()){
            break;
        }

        printk(KERN_INFO "El dispositivo se ha abierto %d veces\n", veces_abierto);
    }

    return 0;
}

/* **************************** */
/* **************************** */

