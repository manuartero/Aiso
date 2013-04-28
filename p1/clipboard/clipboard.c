#include "clipboard.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(modulo_init);
module_exit(modulo_clean);

/* Variables globales */ 
struct proc_dir_entry * directorio_principal;
char* nombre_directorio = "sin _nombre";
extern int periodo;
extern int activo;
/*
struct proc_dir_entry * entrada_clipboard;
struct proc_dir_entry * entrada_selector;
struct proc_dir_entry * entrada_periodo;
*/
struct list_head lista_clipboards;
unsigned int num_clipboards = 5;
unsigned int elemento_actual;

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
  
    directorio_principal = (struct proc_dir_entry *) crear_directorio(nombre_directorio);
    if (directorio_principal == NULL) {error = -1;}
 
    error = crear_lista(); 
    error = crear_entrada(nombre_clipboard, directorio_principal, leer_clipboard, escribir_clipboard);
    error = crear_entrada(nombre_selector, directorio_principal, leer_indice, escribir_indice);
    error = crear_entrada(nombre_periodo, directorio_principal, leer_periodo, escribir_periodo);
    
    if (error != 0) {
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
    elemento_actual = 1;  

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
    int caracteres_copiar;
    printk(KERN_INFO "leer_indice. Seleccionado: %d\n", elemento_actual);
   	caracteres_copiar = snprintf(mi_buff,11,"%d\n",elemento_actual);

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
 * Funcion que se llama cuando leemos del archivo /proc : "cat"
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
 */
int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    int terminado;
    struct clipstruct *seleccionado = NULL;
    printk(KERN_INFO "leer_clipboard. Seleccionado: %d\n", elemento_actual);
    
    /* determinar si hemos terminado de escribir */
    /*preguntar porque entra dos veces en el offset*/
    if (offset > 0) {
    	printk(KERN_INFO "entramos en el offset\n");
        terminado = 0;
    } else {
        /* copiar el contendido del buffer del clipboard en el buffer del sistema */   
        seleccionado = encontrar_clipboard();    
        memcpy(buffer, seleccionado->buffer, seleccionado->num_elem);
        terminado = seleccionado->num_elem;
    }
    
    return terminado;
}

int leer_periodo(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	int terminado;
	char mi_buff[11];
    int caracteres_copiar;
    printk(KERN_INFO "leer_Periodo: %d\n", elemento_actual);
   	caracteres_copiar = snprintf(mi_buff,11,"%d\n",elemento_actual);

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
  
    elemento_actual = nuevo_elemento;
    
    printk(KERN_INFO "Elemento_actual = %d\n", elemento_actual);
    
    //Despertamos al thread
    if (periodo == 0){
    wake_up_process(clipkthread);
    }
    
	//el numeroq pongas en ese return si es menor q el numero de count entonces vulve a llamar a la funcion con lo q le queda
	// return 4;  // tamaño de 1 byte 
    return count;
}

/**
 * Funcion que se llama cuando escribimos en /proc : "echo"
 *
 * @param buffer del sistema cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data)
{
    struct clipstruct *seleccionado;
    printk(KERN_INFO "escribir_clipboard. Seleccionado: %d\n", elemento_actual);
    
    /* encontrar el buffer en el que vamos a escribir */
    seleccionado = encontrar_clipboard();
    printk(KERN_INFO "Encontrado la entrada con id: %d\n", seleccionado->id);
    
    /* copiar en el buffer seleccionado <= buffer */
    seleccionado->num_elem = count;
    if (seleccionado->num_elem > TAM_MAX_BUFFER) {
        seleccionado->num_elem = TAM_MAX_BUFFER;
    }

    if ( copy_from_user(seleccionado->buffer, buffer, seleccionado->num_elem) ) {
        return -EFAULT;
    }
    
  	//Despertamos al thread
   	if (periodo == 0){
   		 wake_up_process(clipkthread);
   	}
    
    printk(KERN_INFO "Salimos de escribir_clipboard\n");
    
    return seleccionado->num_elem;
}

int escribir_periodo(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int nuevo_elemento = 0;
    
    /* transformar el buffer de entrada en int y comprobar q e sun numero correcto*/
	if ((nuevo_elemento = mi_atoi(buffer))== -1)
		return -EINVAL;

    periodo = nuevo_elemento;
    printk(KERN_INFO "Periodo = %d\n", elemento_actual);
    
    //Despertamos al thread
    
    wake_up_process(clipkthread);
 
    
	//el numeroq pongas en ese return si es menor q el numero de count entonces vulve a llamar a la funcion con lo q le queda
	// return 4;  // tamaño de 1 byte 
    return count;
	return 0;
}


// ------------------------------------
// funciones auxiliares
// ------------------------------------


struct clipstruct* encontrar_clipboard(void)
{
    struct clipstruct *tmp = NULL;
    struct list_head *pos;

    list_for_each(pos, &lista_clipboards) {
        tmp = list_entry(pos, struct clipstruct, lista);
        printk(KERN_INFO "id= %d\n", tmp->id);
        if (tmp->id == elemento_actual){
            break;        
        }
     
    	}    
    	if (tmp->id!= elemento_actual)
    		return insertar_nuevo_clipboard();
    		
    return tmp;
}

struct clipstruct* insertar_nuevo_clipboard(void)
{
	struct clipstruct *elemento;
    elemento = (struct clipstruct *) vmalloc( sizeof(struct clipstruct) );
    elemento->id = elemento_actual;
    elemento->num_elem = 0;
    elemento->buffer = (char *) vmalloc( sizeof(TAM_MAX_BUFFER) );
    list_add(&elemento->lista, &lista_clipboards);
    return elemento;
}

