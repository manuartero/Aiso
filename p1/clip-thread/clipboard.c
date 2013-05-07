#include "clipboard.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(modulo_init);
module_exit(modulo_clean);

/* Variables globales */ 
struct proc_dir_entry * directorio_principal;
extern struct proc_dir_entry * directorio_aisoclip;
char* nombre_directorio = "sin_nombre";
extern int periodo;
extern int activo;
struct list_head lista_clipboards;
unsigned int num_clipboards = 5;
extern struct clipstruct *nodo_actual;

// inicializar la lista
LIST_HEAD( lista_clipboards );

// kernel thread
struct task_struct *clipkthread;

// Asignar el numero de clipboards por parametro
module_param(nombre_directorio, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(nombre_directorio, "Nombre del clipboard");

// Asignar el numero de clipboards por parametro
module_param(num_clipboards, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(num_clipboards, "Numero de clipboards");

/* ----------------------------------------------------------- */

/**
 * 1) Crea el directorio con el nombre pasado por argumento
 *   crea la entrada clipboard => inicializar la lista
 *   crea la entrada selection => inicializar el puntero
 *   crea la entrada periodo
 * 2) Lanza el thread
 */
int modulo_init(void)
{
    int error = 0; 
    printk(KERN_INFO "EN MODULO INIT DE CLIPBOARD, nombre = %s \n", nombre_directorio);   
    
    if (directorio_aisoclip == NULL) {
        directorio_aisoclip = crear_directorio(NOMBRE_DIRECTORIO_PRINCIPAL);
    }
    
 	directorio_principal = crear_sub_directorio(nombre_directorio, directorio_aisoclip);
 	
 	if (directorio_principal == NULL) {error = -1;}
    error |= crear_lista(); 
    error |= crear_entrada(nombre_clipboard, directorio_principal, leer_clipboard, escribir_clipboard);
    error |= crear_entrada(nombre_selector, directorio_principal, leer_indice, escribir_indice);
    error |= crear_entrada(nombre_periodo, directorio_principal, leer_periodo, escribir_periodo);
    
    if (error != 0) {
    	printk(KERN_INFO "error\n");
        return -1;
    }    
    
	/* arrancamos el kernel thread */
  	clipkthread = kthread_run(funcion_thread, NULL, "clipkthread");
  	
  	if (clipkthread == (struct task_struct *) ERR_PTR) {
  		return -ENOMEM;
  	}
  	
  	periodo=0;
  	
    return 0;
}


/**
 * Elimina la entrada seleccion
 * Elimina la entrada clipboards
 * Liberar la lista
 * Elimina el directorio
 * Elimina el thread si sigue activo
 */
void modulo_clean(void)
{
	liberar_lista();
    eliminar_sub_entrada(nombre_selector, directorio_principal);
    eliminar_sub_entrada(nombre_clipboard, directorio_principal);
    eliminar_sub_entrada(nombre_periodo, directorio_principal);
    eliminar_entrada(nombre_directorio);
    
    if (activo) {
    	kthread_stop(clipkthread);
  	} else {
    	printk(KERN_INFO "El kernel thread ya no esta activo cuando descargamos el modulo.\n");
  	}
  	
  	printk(KERN_INFO "Modulo descargado.\n");
}

// ------------------------------------------------
// funciones para manipular la lista de clipboards
// ------------------------------------------------

/**
 * Inicializa la lista de clipboards
 *
 * @return (int) exito
 */
int crear_lista(void)
{
    struct clipstruct *elemento;
    
    elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );
    elemento->id = 1;
    elemento->num_elem = 0;
    elemento->buffer = (char *) vmalloc( sizeof(TAM_MAX_BUFFER) );
    list_add(&elemento->lista, &lista_clipboards);

    printk(KERN_INFO "Creada la lista con un unico elemento\n");
    
    /* elemento_actual apunta al primer elemento */ 
	nodo_actual = elemento;
    return 0;
}

/**
 *
 */
void liberar_lista(void)
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
    return;
}


// -------------------------------------------
// funciones de callback
// -------------------------------------------


/** 
 * Funcion que se llama cuando leemos del archivo /proc/aisoclip/selection
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
 */
int leer_indice(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    int terminado;
	char mi_buff[11];
    int id_clipboard;
    printk(KERN_INFO "leer_indice. Seleccionado: %d\n", nodo_actual->id);
   	id_clipboard = snprintf(mi_buff,11,"%d\n",nodo_actual->id);

    /* determinar si hemos terminado de escribir */
    if (offset > 0) {
        terminado = 0;
    } else {
        /* copiar el elemento_actual en el buffer del sistema */ 
        memcpy(buffer, mi_buff, id_clipboard);
        terminado = id_clipboard;
    }
    
    return terminado;
}

/** 
 * Funcion que se llama cuando leemos del archivo /proc : "cat"
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
 */
int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    int terminado;
    printk(KERN_INFO "leer_clipboard. Seleccionado: %d\n", nodo_actual->id);
    
    /* determinar si hemos terminado de escribir */
    /*preguntar porque entra dos veces en el offset*/
    if (offset > 0) {
    	printk(KERN_INFO "entramos en el offset\n");
        terminado = 0;
    } else {
        /* copiar el contendido del buffer del clipboard en el buffer del sistema */   
        
        memcpy(buffer, nodo_actual->buffer, nodo_actual->num_elem);
        terminado = nodo_actual->num_elem;
    }
    
    return terminado;
}

int leer_periodo(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	int terminado;
	char mi_buff[11];
    int caracteres_copiar;
    printk(KERN_INFO "leer_Periodo: %d\n", periodo);
   	caracteres_copiar = snprintf(mi_buff,11,"%d\n",periodo);

    /* determinar si hemos terminado de escribir */
    if (offset > 0) {
        terminado = 0;
    } else {
        /* copiar el elemento_actual en el buffer del sistema */ 
        memcpy(buffer, mi_buff, caracteres_copiar);
        terminado = caracteres_copiar;
    }
    
    return terminado;

}


/**
 * En buffer se almacena la entrada del usuario seguido de un salto de linea
 * Ejemplo: 34 => [3|4|\n|········]
 *  
 * En codigo ASCI 
 *
 * | char | codigo | 
 * |:----:|:------:|
 * | '0'  |  48    |
 * | '1'  |  49    |
 * | '2'  |  50    |
 * | '9'  |  57    |
 * | '\n' |  10    |
 *
 * @param buffer cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
int escribir_indice(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int nuevo_elemento = 0;
    
    /* transformar el buffer de entrada en int y comprobar q e sun numero correcto*/
	if ((nuevo_elemento = mi_atoi(buffer))== -1)
		return -EINVAL;

    /* Comprobar que nos han llamado con un id existente */
    if (nuevo_elemento < 1 || nuevo_elemento > num_clipboards){
    	printk(KERN_INFO "Numero fuera de rango de los clipboard, maximo tamaño = %d\n", num_clipboards);
    	return -EINVAL;
    }
  
  	/* encontrar el buffer en el que vamos a escribir */
    nodo_actual = encontrar_clipboard(nuevo_elemento);
    
    printk(KERN_INFO "Elemento_actual = %d\n", nodo_actual->id);
    
    //Despertamos al thread
    if (periodo == 0){
        wake_up_process(clipkthread);
    }

    return count;
}

/**
 * Funcion que se llama cuando escribimos en /proc/clip/clipboard 
 * @param buffer del sistema cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data)
{
    
    printk(KERN_INFO "escribir_clipboard. Seleccionado: %d\n", nodo_actual->id);
    
    
    /* copiar en el buffer seleccionado <= buffer */
    nodo_actual->num_elem = count;
    if (nodo_actual->num_elem > TAM_MAX_BUFFER) {
        nodo_actual->num_elem = TAM_MAX_BUFFER;
    }

    if ( copy_from_user(nodo_actual->buffer, buffer, nodo_actual->num_elem) ) {
        return -EFAULT;
    }
    
  	//Despertamos al thread
   	if (periodo == 0){
   		 wake_up_process(clipkthread);
   	}
    
    printk(KERN_INFO "Salimos de escribir_clipboard\n");
    
    return nodo_actual->num_elem;
}

int escribir_periodo(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int nuevo_elemento = 0;
    
    /* transformar el buffer de entrada en int y comprobar q e sun numero correcto*/
	if ((nuevo_elemento = mi_atoi(buffer))== -1)
		return -EINVAL;

    periodo = nuevo_elemento;
    printk(KERN_INFO "Periodo = %d\n",nuevo_elemento);
    
    //Despertamos al thread   
    wake_up_process(clipkthread);
 
	return count;
}


// ------------------------------------
// funciones auxiliares
// ------------------------------------

/** 
 * Busca en la lista un elemento que contenga ese id 
 * Si no lo encuentra lo creara. Las restricciones de numero de 
 * clipboards deberan hacerse antes de llamar a esta funcion 
 * @param (int) id de clipboard que queremos buscar
 * @return (struct clipstruct *) nodo encontrado
 */
struct clipstruct* encontrar_clipboard(int id)
{
    struct clipstruct *tmp = NULL;
    struct list_head *pos;

    list_for_each(pos, &lista_clipboards) {
        tmp = list_entry(pos, struct clipstruct, lista);
        printk(KERN_INFO "id= %d\n", tmp->id);
        if (tmp->id == id){
            break;        
        } 
    }    
    
    if (tmp->id!= id) {
        return insertar_nuevo_clipboard(id);
    }
    
    return tmp;
}

struct clipstruct* insertar_nuevo_clipboard(int id)
{
	struct clipstruct *elemento;
    elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );
    elemento->id = id;
    elemento->num_elem = 0;
    elemento->buffer = (char *) vmalloc( sizeof(TAM_MAX_BUFFER) );
    list_add(&elemento->lista, &lista_clipboards);
    return elemento;
}

