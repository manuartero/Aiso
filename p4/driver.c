#include "driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(aiso_init);
module_exit(aiso_exit);

/* Variables globales */
static char * buffer;
static int caracteres_escritos;
static int cabeza_lectura;

static unsigned int veces_abierto;
static dev_t num_mayor_menor; 
int num_mayor;
static unsigned int num_dispositivos = 1; 
static int num_menor = 0;
static struct task_struct *kthread;
static struct cdev * driver_struct;

static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
    .ioctl = aiso_ioctl,
    .llseek = aiso_lseek,
};


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
    caracteres_escritos = 0;
    cabeza_lectura = 0;
    buffer[cabeza_lectura] = '\0';

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
   
	if (*ppos >= caracteres_escritos) {
		return 0;
    }
		
	memcpy(buf, buffer + cabeza_lectura, caracteres_escritos);
	nbytes = caracteres_escritos;
    *ppos = *ppos + caracteres_escritos;

	printk(KERN_INFO "aiso_read => nbytes=%d", nbytes);
	return nbytes;
}

static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = 0;
    nbytes = copy_from_user(buffer + *ppos, buf, lbuf);	
    
    if (nbytes){
        printk(KERN_ALERT "ERROR : aiso_write => copy_from_user \n");    
        return -EFAULT;
    }

    caracteres_escritos = lbuf - nbytes;

	printk(KERN_INFO "aiso_write, caracteres_escritos=%d", caracteres_escritos);
	return caracteres_escritos;
}


/* *************** */
/* Funciones IOCTL */
/* *************** */

extern int aiso_ioctl
(struct inode * inode, struct file * file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int char_leidos;
    loff_t * ppos = vmalloc(sizeof(int));
    char * buffer_entrada;
    unsigned int long_buffer;

    char_leidos = 0;    
    *ppos = 0;
    buffer_entrada = NULL;
    long_buffer = 0;

    switch(ioctl_num){
        case IOCTL_READ: 
            // aiso_read(file, buffer, lon_buffer, posicion)
            char_leidos = aiso_read(file, (char *) ioctl_param, discosize, ppos);
            put_user('\0', (char *) ioctl_param + char_leidos);
        break;

        case IOCTL_WRITE:
            buffer_entrada = (char *) ioctl_param;
            /*            
            get_user(c, buffer_entrada);
            for(char_leidos=0; c && char_leidos<discosize; char_leidos++, buffer_entrada++){
                get_user(c, buffer_entrada);
            }
            */
            long_buffer = (unsigned int) strlen(buffer_entrada);
            aiso_write(file, (char *) ioctl_param, long_buffer, ppos);
        break;

        case IOCTL_LSEEK: 
            aiso_lseek(file, (int) ioctl_param, 0);
        break;

        case IOCTL_RESET: 
            aiso_reset(file);
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
 *   inicio si modo == 2
 * @param loff_t ppos: nueva posicion (2 interpretaciones)
 * @param int modo: determina el comportamiento de ppos
 */
static loff_t aiso_lseek(struct file *file, loff_t ppos, int modo)
{	
    loff_t nueva_pos = -1;

    // calculamos la nueva posicion
    switch(modo) {
        // SEEK_SET  
        case 0: nueva_pos = ppos;
        break;
        
        // SEEK_CUR
        case 1: nueva_pos = file->f_pos + ppos;
        break;
    
        // SEEK_START
        case 2: nueva_pos = 0;
        break;

        // ERROR
        default: return -EINVAL;
    }
    if (nueva_pos < 0) return -EINVAL;
    
    // guardamos la nueva posicion
    file->f_pos = nueva_pos;
    cabeza_lectura = nueva_pos;

    printk(KERN_INFO "LSEEK : %d\n", (int)nueva_pos);
    return nueva_pos;
}

/**
 * Mueve la cabeza de lectura al principio 
 * Escribe el caracter '\0'
 */
static void aiso_reset(struct file * fichero)
{
    fichero->f_pos = 0;
    cabeza_lectura = 0;
    buffer[0] = '\0';
    return;
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

