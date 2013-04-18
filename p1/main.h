/*
 * Practica 1
 *
 * Clipboard multiple con la siguiente funcionalidad:
 *
 * 1) interactuar mediante ficheros virtuales en el directorio /proc/aisoclip 
 *      - tendremos NCLIP clipboards a los que se accedera via /proc/aisoclip/clipboard.
 *      - se eligira el clipboard mediante /proc/aisoclip/selection.
 *      - NCLIP se fijara mediante un parametro. 
 * 
 * el clipboard multiple se implementara con una lista doblemente enlazada de estructuras clipstructs
 */

#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include <linux/list.h>     /* listas del kernel */
#include <asm/uaccess.h>    /* funcion copy_from_user */
#include <linux/vmalloc.h>  /* funcion vmalloc */
#include "clipstruct.h"     /* struct clipstruct */
#include "clipboard.h"      /* funciones lectura y escritura */

#define nombre_directorio "aisoclip"
#define nombre_entrada "clipboard"
#define nombre_selector "selection"
#define TAM 5

extern inline int modulo_aiso_init(void);
extern inline void modulo_aiso_clean(void);
static inline int crear_directorio(void);
static inline int crear_lista(void);
static inline int crear_entrada_clipboard(void);
static inline int crear_entrada_selector(void);
static inline void liberar_lista(void);
static inline void eliminar_entrada(char *entrada);


