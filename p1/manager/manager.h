#ifndef MANAGER_H
#define MANAGER_H

#include <linux/module.h>	            /* modulo */
#include <linux/kernel.h>	            /* kernel */
#include <linux/proc_fs.h>	            /* struct proc_dir_entry */
#include <asm/uaccess.h>                /* function copy_from_user */
#include <linux/moduleparam.h>          /* paso de parametros */
#include <linux/kmod.h>                 /* user mode helper */
#include <linux/list.h>                 /* list head */
#include <linux/vmalloc.h>              /* vmalloc */
#include "../utiles/utiles.h"           /* utiles */

struct nodo_driver {
    char* nombre;
    struct list_head lista;
};
 

// funiones de carga/descarga del modulo
int manager_init(void);

void manager_clean(void);

// funciones de lectura
int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

// funciones de escritura
int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data);

// funciones auxiliares
int add_driver_lista(const char * nuevo_nombre, unsigned long count);

int rm_driver_lista(const char* nombre_nodo);

int eliminar_lista(void);

#endif /* MANAGER_H */

