#include "driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(aiso_init);
module_exit(aiso_exit);

/* Variables globales */
static char * buffer;
int caracteres_escritos;
int caracteres_escritos_tmp;
int cabeza_lectura;
size_t discosize;

static unsigned int veces_abierto;
static dev_t num_mayor_menor; 
int num_mayor;
static unsigned int num_dispositivos = 1; 
static int num_menor = 0;
static struct task_struct *kthread;
static struct cdev * driver_struct;
static struct class * driver_class;

static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
    .llseek = aiso_lseek,
    .ioctl = aiso_ioctl
};


/* ********************* */
/* Funciones init y exit */
/* ********************* */

static int __init aiso_init(void)
{
    discosize = (size_t) (8*PAGE_SIZE);
    // 1) reservamos memoria para el buffer	
    buffer = kmalloc(discosize, GFP_KERNEL);
    
    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menor, num_menor, num_dispositivos, NOMBRE_DEV);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_struct = cdev_alloc();
	cdev_init(driver_struct, &driver_fops);
	cdev_add(driver_struct, num_mayor_menor, num_dispositivos);
	
    // 4) Crear nodo de dispositivo
    driver_class = class_create(THIS_MODULE, NOMBRE_DEV);
    device_create(driver_class, NULL, num_mayor_menor, NULL, NOMBRE_DEV);

    // 5) inicializamos variables
    veces_abierto = 0;  
    num_mayor = MAJOR(num_mayor_menor);
    caracteres_escritos = 0;
    caracteres_escritos_tmp = 0;
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

    // 3) destruir en nodo de dispositivo
    device_destroy(driver_class, num_mayor_menor);
    class_destroy(driver_class);

    // 4) liberamos el buffer	
    kfree(buffer);
	
    // 5) paramos el kthread 
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
    file->f_pos = cabeza_lectura;
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
    if (lbuf+file->f_pos > discosize){
    printk(KERN_INFO "Se sale del rango del fichero\n");
    return -1;
    }

	memcpy(buf, buffer + cabeza_lectura,lbuf);
	
	printk(KERN_INFO "aiso_read => nbytes=%d", lbuf);
	return lbuf;
}

static ssize_t aiso_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes = 0;

    nbytes = copy_from_user(buffer + file->f_pos, buf, lbuf);	
    
    if (nbytes){
        printk(KERN_ALERT "ERROR : aiso_write => copy_from_user \n");    
        return -EFAULT;
    }
	 
    caracteres_escritos_tmp= lbuf - nbytes;

    cabeza_lectura += caracteres_escritos_tmp;         
    file->f_pos += caracteres_escritos_tmp ;
  
    
	printk(KERN_INFO "aiso_write, caracteres_escritos tmp=%d", caracteres_escritos_tmp);
	return caracteres_escritos_tmp;
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
    
    	// no usar
        case IOCTL_READ: 
            //            aiso_read(file, buffer, lon_buffer, posicion)
            char_leidos = aiso_read(file, (char *) ioctl_param, discosize, ppos);
            put_user('\0', (char *) ioctl_param + char_leidos);
            break;

		//no usar
        case IOCTL_WRITE:
            buffer_entrada = (char *) ioctl_param;
            long_buffer = (unsigned int) strlen(buffer_entrada);
            // aiso_write(file, buffer, lon_buffer, posicion)
            aiso_write(file, (char *) ioctl_param, long_buffer, ppos);
            cabeza_lectura += caracteres_escritos_tmp  ; 
            caracteres_escritos += caracteres_escritos_tmp;         
            file->f_pos += caracteres_escritos_tmp ;
            break;
            
		//no usar
        case IOCTL_LSEEK_SET: 
            return aiso_lseek(file, (int) ioctl_param,0);
            break;
         
        //no usar    
        case IOCTL_LSEEK_CURR: 
            return aiso_lseek(file, (int) ioctl_param,1);
            break;
                 
        //no usar        
        case IOCTL_LSEEK_END: 
            return aiso_lseek(file, (int) ioctl_param,2);
            break;
		         
        //no usar    
        case IOCTL_RESET: 
            aiso_reset(file);
            break;
		
        case IOCTL_WRITTEN:
            aiso_state(file, (int *) ioctl_param, 2);
            break;

        case IOCTL_POINTER:
            aiso_state(file, (int *) ioctl_param, 1);
            break;

        case IOCTL_MODIFY:
            return aiso_modify(file, (int) ioctl_param);
            break;

        default: return -ENOTTY;
    }

    return 0;
}

/**
 * Posiciona el puntero
 */
static int aiso_lseek(struct file *file, loff_t pos,int whence)
{	
/*    file->f_pos += pos;*/
/*    cabeza_lectura = pos;*/

/*    printk(KERN_INFO "LSEEK : %d\n", file->f_pos);*/
/*    return pos;*/
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = pos;
		break;

	  case 1: /* SEEK_CUR */
		newpos = file->f_pos + pos;
		break;

	  case 2: /* SEEK_END */
		newpos = discosize + pos;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	file->f_pos = newpos;
	cabeza_lectura = newpos;
	return newpos;
}

/**
 * Mueve la cabeza de lectura al principio
 * Escribe 0 en caracteres_escritos 
 * Escribe el caracter '\0'
 */
static void aiso_reset(struct file * fichero)
{
    fichero->f_pos = 0;
    cabeza_lectura = 0;
    caracteres_escritos = 0;
    buffer[0] = '\0';
    return;
}

/**
 * Modifica el tam del buffer
 * Implicitamente hace Reset
 */
static int aiso_modify(struct file * fichero, size_t nueva_cantidad)
{
    // 1) Liberamos el buffer actual
    kfree(buffer);

    discosize = nueva_cantidad;
    // 2) Reservamos un nuevo buffer	
    buffer = kmalloc(discosize, GFP_KERNEL);

    // 3) Reseteamos el valor de las variables
    caracteres_escritos = 0;
    caracteres_escritos_tmp = 0;
    cabeza_lectura = 0;
    buffer[cabeza_lectura] = '\0';

    if(buffer == NULL){
        return -1;
    }
    return 0;
}

/**
 * Guarda en n el numero consultado
 * mode == 1 : cabeza_lectura
 * mode == 2 : caracteres_escritos (default)
 */
static void aiso_state(struct file * fichero, int * n, int mode)
{
    if(mode == 1){
        *n = cabeza_lectura;
    } else { 
        *n = caracteres_escritos;
    }
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

