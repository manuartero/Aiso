#include "manager.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(manager_init);
module_exit(manager_clean);

extern struct proc_dir_entry * directorio_aisoclip;
struct list_head lista_drivers;
LIST_HEAD( lista_drivers );
int numero_drivers = 0;

int manager_init(void)
{
    int error = 0;
    
    if (directorio_aisoclip == NULL) {
      directorio_aisoclip = crear_directorio(NOMBRE_DIRECTORIO_PRINCIPAL);
    }
    
    error |= crear_entrada("activar", directorio_aisoclip, leer_activar, escribir_activar);
    error |= crear_entrada("desactivar", directorio_aisoclip, leer_desactivar, escribir_desactivar);
    error |= crear_entrada("monitor", directorio_aisoclip, leer_monitor, escribir_monitor);

    return error;
}

void manager_clean(void)
{
	
    eliminar_sub_entrada("monitor", directorio_aisoclip);
    eliminar_sub_entrada("desactivar", directorio_aisoclip);
    eliminar_sub_entrada("activar", directorio_aisoclip);
    eliminar_entrada(NOMBRE_DIRECTORIO_PRINCIPAL);
}

/** sin efecto */
int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    return 0;
}

/** sin efecto */
int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    return 0;
}

/**
 * Mostrar los nombres de los clips cargados
 */
int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    struct nodo_driver *tmp = NULL;
    struct list_head *pos;
    int terminado=0;
    size_t longitud=0;
    char * nombres_drivers= vmalloc(4096);
    
    // recorrer la lista: rellenar nombres_drivers 
    
    
    // determinar si hemos terminado de escribir
    if (offset > 0) {
        terminado = 0;
    } else {
        // copiar el elemento_actual en el buffer del sistema 
        //FIXME
        list_for_each(pos, &lista_drivers) {
        tmp = list_entry(pos, struct nodo_driver, lista);
      	longitud =strlen(tmp->nombre);
      	terminado = terminado+longitud;
      	strcat(nombres_drivers,tmp->nombre);
      	//salto de linea
      	nombres_drivers[terminado]=10;
      	terminado++;
      	//zu es para size_t
      	//printk(KERN_INFO "tamaño de la cosa %zu\n",longitud);
      	//printk(KERN_INFO "%s\n",nombres_drivers);
        }
        memcpy(buffer,nombres_drivers, terminado);
    }
    
    return terminado;
}

/**
 * El usuario escribira en el buffer el nombre con el que queremos instalar el nuevo modulo 
 * La funcion llamara al programa <modprobe> para instalar el modulo <clip> con el nombre especificado
 * El modulo <clip> tendra que estar en la carpeta /lib/modules/2.6.32-21-generic
 * (ejecutar el comando $> depmod -a)
 * TODO: considerar que introduzco <nombre basura> 
 */
 int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int num_caracteres = 19 + count;
    char argumento_nombre[num_caracteres];
    char *nombre_introducido= NULL;    
    int error = 0;
    char *argv[] = { "/sbin/modprobe", "-o", nombre_introducido, "clip", argumento_nombre, NULL}; 
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    
    nombre_introducido = (char *) vmalloc(count);
    
    // 1) recoger el nombre del modulo    
    if ( copy_from_user(nombre_introducido, buffer, count) ) {
        return -EFAULT;
    }
    
    argv[2]=nombre_introducido;
    
    add_driver_lista(nombre_introducido,count);
    
    // 2) ejecutar modprobe
    strcpy(argumento_nombre, "nombre_directorio=");
    strcat(argumento_nombre, nombre_introducido);
    
    
    error = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
    if(error){
        return -1;
    } 
    
    numero_drivers++;
    return count;
} 

/**
 * Eliminar el clip seleccionado
 */
int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char *nombre_introducido = vmalloc(count);    
    int error = 0;
    int encontrado;
    char *argv[] = { "/sbin/modprobe", "-o", nombre_introducido, "clip", NULL}; 
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    
    // 1) recoger el nombre del modulo    
    if ( copy_from_user(nombre_introducido, buffer, count) ) {
        return -EFAULT;
    }
    
    encontrado = rm_driver_lista(nombre_introducido); 
  	if (!encontrado){
		printk(KERN_INFO "no existe el clipboard %s\n",nombre_introducido);	
   	}
    
    error = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
    if (error){
        return -1;
    }
    
    return count;
}

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // Sin efecto
    return 0;
}

// funciones auxiliares

int add_driver_lista(const char * nuevo_nombre, unsigned long count)
{
    struct nodo_driver * elemento;
   
    elemento = (struct nodo_driver *) vmalloc( sizeof(struct nodo_driver) );
    elemento->nombre = (char *) vmalloc( sizeof(char)*(count) );
    strncpy(elemento->nombre, nuevo_nombre,count); 
     
    list_add(&elemento->lista, &lista_drivers);

    return 0;
}


int eliminar_lista(void)
{
    struct list_head *pos, *q;
    struct nodo_driver *tmp;

    list_for_each_safe(pos, q, &lista_drivers){
        tmp = list_entry(pos, struct nodo_driver, lista);
        printk("liberamos el nodo: %s |n ", tmp->nombre);
        vfree(tmp->nombre);
        vfree(tmp);
        list_del(pos);            
    }

    return 0;
}

/**
 * @return 0 si no borra ningun nodo 
 * @return 1 si borra un nodo
 */

int rm_driver_lista(const char * nombre_nodo)
{
    struct list_head *pos, *q;
    struct nodo_driver *tmp;
    int borrado = 0;

    list_for_each_safe(pos, q, &lista_drivers){
        tmp = list_entry(pos, struct nodo_driver, lista);
        if ( strcmp(tmp->nombre, nombre_nodo)==0 ) {
            printk("liberamos el nodo: %s\n", tmp->nombre);
            vfree(tmp);
            list_del(pos);
            borrado = 1;
            break;
        }
    }

    return borrado;
}

