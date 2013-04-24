#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include <linux/list.h>     /* listas del kernel */
#include <linux/vmalloc.h>  /* funcion vmalloc */
#include <asm/uaccess.h>    /* function copy_from_user */
#include "clipstruct.h"     /* struct clipstruct */
#include "utiles.h"          /* funciones utiles */

/* Declaracion de macros */
#define nombre_directorio "aisoclip"
#define nombre_entrada "clipboard"
#define nombre_selector "selection"
#define CAMBIO_CLIPBOARD 4
#define ESCRITURA_CLIPBOARD 5
#define TAM_MAX_BUFFER 4096

/* Declaracion de funciones */
int leer_indice(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
int modificar_indice(struct file *file, const char *buffer, unsigned long count, void *data);
int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data);
int crear_directorio(void);
int crear_lista(void);
int crear_entrada_clipboard(void);
int crear_entrada_selector(void);
void liberar_lista(void);
inline void eliminar_entrada(char *entrada, struct proc_dir_entry *parent);

/* Declaracion de funciones auxiliares */
struct clipstruct* encontrar_clipboard(void);
struct clipstruct* insertar_nuevo_clipboard(void);

#endif /* CLIPBOARD_H */

