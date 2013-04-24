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
LIST_HEAD( lista_clipboards );
struct task_struct *clipkthread;
int activo;

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
			if (current == ESCRITURA_CLIPBOARD) {
				printk(KERN_INFO "THREAD : ESCRITURA_CLIPBOARD.\n");
			} else if (current == CAMBIO_CLIPBOARD) {
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

// ---------------------------------------------------------
// Funciones auxiliares
// ---------------------------------------------------------

static int crear_directorio(void)
{
    directorio = proc_mkdir(nombre_directorio, NULL); 	
    
    // comprobacion de errores
	if (directorio == NULL) {
		remove_proc_entry(nombre_directorio, NULL);
		printk(KERN_ALERT "Error: No se pudo crear el directorio /%s\n", nombre_directorio);
		return -ENOMEM;
	}

    printk(KERN_INFO "Creado el directorio /proc/%s \n", nombre_directorio);
    return 0;
}

//funcionq inicialzia toda la lista de clipboards
/*static int crear_lista(void)*/
/*{*/
/*    struct clipstruct *elemento;*/
/*    int i;*/
/*    */
/*    for (i=1; i<=tam; i++){*/
/*        elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );*/
/*        elemento->id = i;*/
/*        elemento->num_elem = 0;*/
/*        elemento->buffer = (char *) vmalloc( sizeof(TAM_MAX_BUFFER) );*/
/*        list_add(&elemento->lista, &lista_clipboards);*/
/*    }*/

/*    printk(KERN_INFO "Creada la lista \n");*/
/*    */
/*     elemento_actual apunta al primer elemento */
/*    elemento_actual = 1;  */
/*    //printk("elemento_actual apunta a %d")    */
/*    */
/*    lista_clipboards es una constante */
/*      pos =  &lista_clipboards;   */
/*        elemento_actual = list_entry(pos, struct clipstruct, lista);*/
/*        printk(KERN_INFO "elemento_actual tiene: %d", elemento_actual->id);*/
/*    */
/*    */
/*    return 0;*/
/*}*/

static int crear_lista(void)
{
    struct clipstruct *elemento;
    
    elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );
    elemento->id = 1;
    elemento->num_elem = 0;
    elemento->buffer = (char *) vmalloc( sizeof(TAM_MAX_BUFFER) );
    list_add(&elemento->lista, &lista_clipboards);

    printk(KERN_INFO "Creada la lista con un unico elemento\n");
    
    /* elemento_actual apunta al primer elemento */
    elemento_actual = 1;  

    
    return 0;
}

static int crear_entrada_clipboard(void)
{
    /* creamos la entrada principal */
    entrada_proc = create_proc_entry(nombre_entrada, 0644, directorio);
    
    /* Rellenar la estructura */
    entrada_proc->read_proc     = leer_clipboard;
    entrada_proc->write_proc    = escribir_clipboard;
    entrada_proc->mode 	        = S_IFREG | S_IRUGO;
    entrada_proc->uid 	        = 0; //id user: default 0 
    entrada_proc->gid 	        = 0; //group user: default 0
   
    printk(KERN_INFO "Creada la entrada /%s/%s \n", nombre_directorio, nombre_entrada);
    return 0;
}

static int crear_entrada_selector(void)
{
    /* creamos la entrada seleccion */
    selector = create_proc_entry(nombre_selector, 0644, directorio);
    
    /* Rellenar la estructura */
    selector->read_proc     = leer_indice;
    selector->write_proc    = modificar_indice;
    selector->mode 	      = S_IFREG | S_IRUGO;
    selector->uid 	      = 0; 
    selector->gid 	      = 0;
   
    printk(KERN_INFO "Creada la entrada /proc/%s/%s \n", nombre_directorio, nombre_selector);
	
	return 0;
}

static void liberar_lista(void)
{
    struct list_head *pos, *q;
    struct clipstruct *tmp;
    list_for_each_safe(pos, q, &lista_clipboards){
        tmp = list_entry(pos, struct clipstruct, lista);
        vfree(tmp->buffer);
        printk("liberamos el nodo: %d | ", tmp->id);
        vfree(tmp);        
        list_del(pos);
    }
    printk("Lista eliminada \n");
    return;
}

static inline void eliminar_entrada(char *entrada, struct proc_dir_entry *parent)
{
    remove_proc_entry(entrada, parent);
    printk(KERN_INFO "Eliminada la entrada %s", entrada);
}

