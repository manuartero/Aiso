/*
 *  proc1.c
 *
 * Ejemplo basico de uso de /proc 
 * 1) Crear un archivo en /proc/aiso en la funcion <init_module>,
 * 2) Devolver un valor y un buffer cuando leemos la entrada /proc/helloworld en la funcion callback <procfs_read>
 * 3) Borrar el archivo /proc/aiso en la function <cleanup_module>.
 */

#include <linux/module.h>	/* Specifically, a module */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */
#include <asm/uaccess.h>    /* copy_from_user */

#define procfs_name "aiso"
#define MAX_SIZE    4096

/*
 * struct proc_dir_entry{
 * ...
 * read_proc_t read_proc;  => read callback
 * write_proc *write_proc; => write callback
 * ...
 * };
 */
static struct proc_dir_entry *entrada_proc;

static char buffer_copia[MAX_SIZE];
static unsigned long buffer_size = 0;

/* Put data into the proc fs file.
 * 
 * Arguments
 * =========
 * 1. The buffer where the data is to be inserted, if
 *    you decide to use it.
 * 2. A pointer to a pointer to characters. This is
 *    useful if you don't want to use the buffer
 *    allocated by the kernel.
 * 3. The current position in the file
 * 4. The size of the buffer in the first argument.
 * 5. Write a "1" here to indicate EOF.
 * 6. A pointer to data (useful in case one common 
 *    read for multiple /proc/... entries)
 *
 * Usage and Return Value
 * ======================
 * A return value of zero means you have no further
 * information at this time (end of file). A negative
 * return value is an error condition.
 *
 */ 

/* 
 * This function is called then the /proc file is read: cat
 */
int read_proc(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;
	
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);
	
	if (offset > 0) {
		// Hemos terminado de leer
		ret  = 0;
	} else {
	    // copiamos el contendio de <buffer_copia> en <buffer>
		memcpy(buffer, buffer_copia, buffer_size);
		ret = buffer_size;
	}

	return ret;
}

/**
 * This function is called with the /proc file is written: echo
 *
 * @2
 * @3
 */
int write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	/* get buffer size */
	buffer_size = count;
	if (buffer_size > MAX_SIZE ) {
		buffer_size = MAX_SIZE;
	}
	
	/* write data to the buffer */
	if ( copy_from_user(buffer_copia, buffer, buffer_size) ) {
		return -EFAULT;
	}
	
	return buffer_size;
}

/*
 * Crear la entrada_proc y rellenar la estructura, indicando
 * Que funcion se llamara en el read: read_proc
 * Que funcion se llamara en el write: write_proc
 */
int modulo_aiso_init(void)
{
	entrada_proc = create_proc_entry(procfs_name, 0644, NULL);
	
	if (entrada_proc == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
		       procfs_name);
		return -ENOMEM;
	}

    /* Rellenar la estructura */
	entrada_proc->read_proc     = read_proc;
	entrada_proc->write_proc    = write_proc;
	entrada_proc->mode 	        = S_IFREG | S_IRUGO;
	entrada_proc->uid 	        = 0; //id user: default 0 
	entrada_proc->gid 	        = 0; //group user: default 0
	entrada_proc->size 	        = 0;

	printk(KERN_INFO "Creada la entrada /proc/%s \n", procfs_name);	
	return 0;
}

void modulo_aiso_clean(void)
{
	remove_proc_entry(procfs_name, NULL);
	printk(KERN_INFO "Eliminada la entrada /proc/%s \n", procfs_name);
}

// Declaracion de funciones <init> y <exit>
module_init(modulo_aiso_init);
module_exit(modulo_aiso_clean);
