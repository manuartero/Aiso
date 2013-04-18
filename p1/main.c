#include "main.h"
MODULE_LICENSE("GPL");
module_init(modulo_aiso_init);
module_exit(modulo_aiso_clean);

static struct proc_dir_entry *directorio;
static struct proc_dir_entry *entrada_proc;
static struct proc_dir_entry *indice;
LIST_HEAD( lista_clipboards );
static struct clipstruct *elemento_actual;


/* ================================================================ */


/**
 * crear el directior aisoclip
 *   crear la entrada clipboard => inicializar la lista
 *   crear la entrada selection => inicializar el puntero
 * 
 * inicializar la lista
 */
extern int modulo_aiso_init(void)
{
    int error;    
    
    error = crear_directorio();
    error = crear_lista(); 
    error = crear_entrada_clipboard(); 
    error = crear_entrada_selector();

    if (error != 0) {
        return -1;
    }    

    return 0;
}

/**
 * Eliminar la entrada seleccion
 * Eliminar la entrada clipboards
 * Liberar la lista
 * Eliminar el directorio
 */
extern void modulo_aiso_clean(void)
{
	liberar_lista();
    eliminar_entrada(nombre_selector);
    eliminar_entrada(nombre_entrada);
    eliminar_entrada(nombre_directorio);
}

// ---------------------------------------------------------
// Funciones auxiliares
// ---------------------------------------------------------

static inline int crear_directorio(void)
{
    directorio = proc_mkdir(nombre_directorio, NULL); 	
    
    // comprobacion de errores
	if (directorio == NULL) {
		remove_proc_entry(nombre_directorio, NULL);
		printk(KERN_ALERT "Error: No se pudo crear el directorio /proc/%s\n", nombre_directorio);
		return -ENOMEM;
	}

    printk(KERN_INFO "Creado el directorio /proc/%s \n", nombre_directorio);
    return 0;
}

static inline int crear_lista(void)
{
    struct clipstruct *elemento;    
    int i;
    
    for (i=1; i<=TAM; i++){
        elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );
        elemento->id = i;
        elemento->buffer = (char *) vmalloc( sizeof(TAM_BUFFER) );
        list_add(&elemento->lista, &lista_clipboards);
    }

    /* elemento_actual apunta al primer elemento */
    /*  TODO
    elemento_actual = list_entry(lista_clipboards, struct clipstruct, lista);
    printk(KERN_INFO "elemento_actual tiene: %d", elemento_actual->id);
    */

    printk(KERN_INFO "Creada la lista \n");
    return 0;
}

static inline int crear_entrada_clipboard(void)
{
    /* creamos la entrada principal */
    entrada_proc = create_proc_entry(nombre_entrada, 0644, directorio);
    
    /* Rellenar la estructura */
    entrada_proc->read_proc     = leer_clipboard;
    entrada_proc->write_proc    = escribir_clipboard;
    entrada_proc->mode 	        = S_IFREG | S_IRUGO;
    entrada_proc->uid 	        = 0; //id user: default 0 
    entrada_proc->gid 	        = 0; //group user: default 0
   
    printk(KERN_INFO "Creada la entrada /proc/%s/%s \n", nombre_directorio, nombre_entrada);
    return 0;
}

static inline int crear_entrada_selector(void)
{
    /* creamos la entrada seleccion */
    indice = create_proc_entry(nombre_selector, 0644, directorio);
    
    /* Rellenar la estructura */
    indice->read_proc     = leer_indice;
    indice->write_proc    = modificar_indice;
    indice->mode 	      = S_IFREG | S_IRUGO;
    indice->uid 	      = 0; 
    indice->gid 	      = 0;
   
    printk(KERN_INFO "Creada la entrada /proc/%s/%s \n", nombre_directorio, nombre_selector);
	
	return 0;
}

static inline void liberar_lista(void)
{
    struct list_head *pos, *q;
    struct clipstruct *tmp;
    list_for_each_safe(pos, q, &lista_clipboards){
        tmp = list_entry(pos, struct clipstruct, lista);
        printk("liberamos el nodo: %d | ", tmp->id);
        list_del(pos);
        vfree(tmp);
    }
    printk("Lista eliminada \n");
    return;
}

static inline void eliminar_entrada(char *entrada)
{
    remove_proc_entry(entrada, NULL);
    printk(KERN_INFO "Eliminada la entrada %s", entrada);
}

