#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <linux/module.h>	        /* modulo */
#include <linux/kernel.h>	        /* kernel */
#include <linux/proc_fs.h>	        /* struct proc_dir_entry */
#include <linux/list.h>             /* listas del kernel */
#include <linux/vmalloc.h>          /* funcion vmalloc */
#include <asm/uaccess.h>            /* function copy_from_user */
#include <linux/moduleparam.h>      /* paso de parametros */
#include "clipstruct.h"             /* struct clipstruct */
#include "utiles.h"                 /* funciones utiles */

/* Declaracion de macros */
#define nombre_entrada "clipboard"
#define nombre_selector "selection"
#define nombre_periodo "periodo"
#define CAMBIO_CLIPBOARD 4
#define ESCRITURA_CLIPBOARD 5
#define TAM_MAX_BUFFER 4096

/* Declaracion de funciones */

int modulo_init(void);

void modulo_clean(void);


// funciones para crear-destruir estradas o directorios

int crear_directorio(const char * nombre_directorio);

int crear_sub_directorio(const char * nombre_directorio, struct proc_dir_entry * directorio_padre);

int crear_entrada(const char* nombre_entrada, struct proc_dir_entry *directorio,
 int (*leer) (char*, char**, off_t, int, int*, void*),
 int (*escribir) (struct file*, const char*, unsigned long, void*)
); 
    

inline void eliminar_entrada(char * entrada);

inline void eliminar_sub_entrada(char * entrada, struct proc_dir_entry *parent);

int crear_lista(void);

void liberar_lista(void);


// funciones de callback

int leer_indice(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_periodo(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int escribir_indice(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_periodo(struct file *file, const char *buffer, unsigned long count, void *data);


// funciones auxiliares

struct clipstruct* encontrar_clipboard(void);

struct clipstruct* insertar_nuevo_clipboard(void);

#endif /* CLIPBOARD_H */

