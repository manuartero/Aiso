#include "main.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(modulo_aiso_init);
module_exit(modulo_aiso_clean);

/* Variables globales */ 

struct proc_dir_entry *directorio;
struct proc_dir_entry *entrada_proc;
struct proc_dir_entry *selector;
struct list_head lista_clipboards;
unsigned int num_clipboards = 5;
unsigned int elemento_actual;

// inicializar la lista
LIST_HEAD( lista_clipboards );

// kernel thread
struct task_struct *clipkthread;
int activo;
int ticket;

// Asignar el numero de clipboards por parametro
module_param(num_clipboards, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(num_clipboards, "Numero de clipboards");

/* ----------------------------------------------------------- */

/**
 * crear el directior aisoclip
 *   crear la entrada clipboard => inicializar la lista
 *   crear la entrada selection => inicializar el puntero
 * 
 * inicializar la lista
 * lanza el thread
 */
int modulo_aiso_init(void)
{
    int error;    
     
    error = crear_directorio();
    error = crear_lista(); 
    error = crear_entrada_clipboard(); 
    error = crear_entrada_selector();
	
    if (error != 0) {
        return -1;
    }    
    
	/* arrancamos el kernel thread */
  	clipkthread = kthread_run(escritura_thread, NULL,"escritura_thread");
  	
  	if (clipkthread == (struct task_struct *) ERR_PTR) {
  		return -ENOMEM;
  	}
  	
    return 0;
}

int escritura_thread(void *data) 
{
	allow_signal(SIGKILL);
	printk(KERN_INFO "iniciando ejecuacion kernel thread.\n");
	activo = 1;	  
	
	for(;;){
		msleep(100);
		if (kthread_should_stop()) {
			activo = 0;
			break;
		}
		
		if (signal_pending(current)){
			if (ticket == ESCRITURA_CLIPBOARD) {
				printk(KERN_INFO "THREAD : ESCRITURA_CLIPBOARD.\n");
			} else if (ticket == CAMBIO_CLIPBOARD) {
				printk(KERN_INFO "THREAD: CAMBIO_CLIPBOARD.\n");
			} else {
				 /* señal desconocida */
				 activo = 0;
				 break;
			}
		}
		
	}
	printk(KERN_INFO "finalizando ejecucion kernel thread.\n");
	return 0;
}

/**
 * Eliminar la entrada seleccion
 * Eliminar la entrada clipboards
 * Liberar la lista
 * Eliminar el directorio
 * Eliminar el thread si sigue activo
 */
void modulo_aiso_clean(void)
{
	liberar_lista();
    eliminar_entrada(nombre_selector, directorio);
    eliminar_entrada(nombre_entrada, directorio);
    eliminar_entrada(nombre_directorio, NULL);
    
    if (activo) {
    	kthread_stop(clipkthread);
  	} else {
    	printk(KERN_INFO "El kernel thread ya no esta activo cuando descargamos el modulo.\n");
  	}
  	
  	printk(KERN_INFO "Modulo descargado.\n");
}

