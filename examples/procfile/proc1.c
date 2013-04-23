/*
 *  proc1.c
 *
 * Ejemplo basico de uso de /proc 
 * 1) Crear un archivo en /proc/aiso en la funcion <init_module>,
 * 2) Copiar una cadena en el buffer en la llamada <write>
 * 3) Volvar el contentido del buffer en la llamada <read>
 * 4) Borrar el archivo /proc/aiso en la function <cleanup_module>.
 */

#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include <asm/uaccess.h>    /* function copy_from_user */

#define procfs_name "aiso"
#define MAX_SIZE    4096

/*
 * struct proc_dir_entry {
 *   ...
 *   read_proc_t read_proc;  => read callback
 *   write_proc *write_proc; => write callback
 *   ...
 * };
 */
static struct proc_dir_entry *entrada_proc;

static char buffer_copia[MAX_SIZE];
static unsigned long buffer_size = 0;

/** 
 * Funcion que se llama cuando leemos del archivo /proc : "cat"
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
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
 * Funcion que se llama cuando escribimos en /proc : "echo"
 *
 * @param buffer cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
int write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	/* copiamos como maximo MAX_SIZE caracteres */
	buffer_size = count;
	if (buffer_size > MAX_SIZE ) {
		buffer_size = MAX_SIZE;
	}
	
	/* copiamos el contendio de <buffer> en <buffer_copia> */
	if ( copy_from_user(buffer_copia, buffer, buffer_size) ) {
		return -EFAULT;
	}
	
	return buffer_size;
}

/*
 * Crear la entrada_proc y rellenar la estructura, indicando
 * Que funcion se llamara en el read    => read_proc
 * Que funcion se llamara en el write   => write_proc
 */
int modulo_aiso_init(void)
{
	entrada_proc = create_proc_entry(procfs_name, 0644, NULL);
	
	if (entrada_proc == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: No se pudo inicializar la entrada /proc/%s\n", procfs_name);
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
