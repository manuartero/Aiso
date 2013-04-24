#include "clipboard.h"

/* Variables globales */
extern struct proc_dir_entry *directorio;
extern struct proc_dir_entry *entrada_proc;
extern struct proc_dir_entry *selector;
extern struct list_head lista_clipboards;
extern unsigned int elemento_actual;
extern unsigned int num_clipboards;

int leer_periodo(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	// TODO
}

int escribir_periodo(struct file *file, const char *buffer, unsigned long count, void *data)
{
	// TODO
}

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

     printk(KERN_INFO "lo q tiene el bufer en 0 %s\n",mi_buff);
    /* determinar si hemos terminado de escribir */
    if (offset > 0) {
        terminado = 0;
    } else {
        /* copiar el elemento_actual en el buffer del sistema */ 
        printk(KERN_INFO "entramos a copiar \n");
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
int modificar_indice(struct file *file, const char *buffer, unsigned long count, void *data)
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
    
    printk(KERN_INFO "Salimos de escribir_clipboard\n");
    
    return seleccionado->num_elem;
}

struct clipstruct* encontrar_clipboard(void)
{
    struct clipstruct *tmp = NULL;
    struct list_head *pos;
    //boolean delante = true;

    list_for_each(pos, &lista_clipboards) {
        tmp = list_entry(pos, struct clipstruct, lista);
        printk(KERN_INFO "id= %d\n", tmp->id);
        if (tmp->id == elemento_actual){
            break;        
        }
      /*  if (tmp->id > elemento_actual)
        	delante = false;*/
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

int crear_directorio(const char* nombre_directorio)
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

/**
 * Inicializa la lista de clipboards
 *
 * @return int exito
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
 * Crea la entrada especificada en el directorio /proc/directorio especificado
 *
 * @return int exito
 */
int crear_entrada( const char * nombre_entrada, const char * nombre_directorio, struct proc_dir_entry *directorio,
							 int (*leer) (char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data),
							 int (*escribir) (struct file *file, const char *buffer, unsigned long count, void *data) )
{
    /* creamos la entrada principal */
    entrada_proc = create_proc_entry(nombre_entrada, 0644, directorio);
    
    /* Rellenar la estructura */
    entrada_proc->read_proc     = leer;
    entrada_proc->write_proc    = escribir;
    entrada_proc->mode 	        = S_IFREG | S_IRUGO;
    entrada_proc->uid 	        = 0; // id user 
    entrada_proc->gid 	        = 0; //group user
   
    printk(KERN_INFO "Creada la entrada /%s/%s \n", nombre_directorio, nombre_entrada);
    return 0;
}

/*
int crear_entrada_selector(void)
{
    // creamos la entrada seleccion 
    selector = create_proc_entry(nombre_selector, 0644, directorio);
    
    // Rellenar la estructura
    selector->read_proc     = leer_indice;
    selector->write_proc    = modificar_indice;
    selector->mode 	      = S_IFREG | S_IRUGO;
    selector->uid 	      = 0; 
    selector->gid 	      = 0;
   
    printk(KERN_INFO "Creada la entrada /proc/%s/%s \n", nombre_directorio, nombre_selector);
	
	return 0;
}
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
    printk("Lista eliminada \n");
    return;
}

inline void eliminar_entrada(char *entrada, struct proc_dir_entry *parent)
{
    remove_proc_entry(entrada, parent);
    printk(KERN_INFO "Eliminada la entrada %s", entrada);
}

