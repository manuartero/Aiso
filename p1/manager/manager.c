#include "manager.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(manager_init);
module_exit(manager_clean);

#define NOMBRE_DIRECTORIO_PRINCIPAL "aisoclip"

struct proc_dir_entry * directorio_aisoclip;
struct list_head lista_drivers;
LIST_HEAD( lista_drivers );

int manager_init(void)
{
    int error = 0;

    directorio_aisoclip = crear_directorio(NOMBRE_DIRECTORIO_PRINCIPAL);
    if(directorio_aisoclip == NULL){error = -1;}
    
    error = crear_entrada("activar", directorio_aisoclip, leer_activar, escribir_activar);
    error = crear_entrada("desactivar", directorio_aisoclip, leer_desactivar, escribir_desactivar);
    error = crear_entrada("monitor", directorio_aisoclip, leer_monitor, escribir_monitor);

    return error;
}

void manager_clean(void)
{
    eliminar_sub_entrada("monitor", directorio_aisoclip);
    eliminar_sub_entrada("desactivar", directorio_aisoclip);
    eliminar_sub_entrada("activar", directorio_aisoclip);
}

int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // Sin efecto
    return 0;
}

int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // Sin efecto
    return 0;
}

int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO
    return 0;
}

/**
 * El usuario escribira en el buffer el nombre con el que queremos instalar el nuevo modulo 
 * La funcion llamara al programa <modprobe> para instalar el modulo <clip.ko> con el nombre especificado
 * El modulo <clip.ko> tendra que estar en la carpeta /lib/modules/2.6.32-21-generic
 * (ejecutar el comando $> depmod -a)
 */ 
int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    struct subprocess_info * sub_info = NULL;
    char *argv[] = { "/sbin/modprobe","clip.ko"}; 
    char *envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char *nombre = NULL; //TODO: considerar que introduzco <nombre parametro>

    // 1) recoger el nombre del modulo    
    if ( copy_from_user(nombre, buffer, count) ) {
        return -EFAULT;
    }
    if (nombre == NULL) {
        return -2;
    }

    // 2) llamada al programa modprobe TODO: argv[4] = NOMBRE_DIRECTORIO_PRINCIPAL
    argv[2] = "-o";
    argv[3] = nombre;
    argv[4] = nombre; 
    sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC );

    if (sub_info == NULL) {
        return -ENOMEM;
    }

    return call_usermodehelper_exec( sub_info, UMH_WAIT_PROC );
}

int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // TODO
    return 0;
}

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // Sin efecto
    return 0;
}

// funciones auxiliares

int add_driver_lista(const char * nuevo_nombre)
{
    struct nodo_driver * elemento;
    size_t length_nombre;

    elemento = (struct nodo_driver *) vmalloc( sizeof(struct nodo_driver) );
    //FIXME: revisar que esto funciona
    length_nombre = strlen(nuevo_nombre);
    strncpy(elemento->nombre, nuevo_nombre, length_nombre);  
    list_add(&elemento->lista, &lista_drivers);

    printk(KERN_INFO "nuevo nodo en la lista de drivers: %s\n", elemento->nombre);
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
            vfree(tmp);
            list_del(pos);            
            printk("liberamos el nodo: %s\n", tmp->nombre);
            borrado = 1;
            break;
        } 
    }

    return borrado;
}
