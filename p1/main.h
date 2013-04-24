#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include <linux/list.h>     /* listas del kernel */
#include <linux/vmalloc.h>  /* funcion vmalloc */
#include "clipstruct.h"     /* struct clipstruct */
#include "clipboard.h"      /* funciones lectura y escritura */
#include <linux/moduleparam.h>  // paso de parametros
#include <linux/init.h>         // macros
#include <linux/stat.h>         //¿permisos?

/* Definicio de macros */
#define nombre_directorio "aisoclip"
#define nombre_entrada "clipboard"
#define nombre_selector "selection"
//#define TAM 5

/* Declaracion de funciones */
int modulo_aiso_init(void);
void modulo_aiso_clean(void);

/* Declaracion de funciones auxiliares */
static int crear_directorio(void);
static int crear_lista(void);
static int crear_entrada_clipboard(void);
static int crear_entrada_selector(void);
static void liberar_lista(void);
static inline void eliminar_entrada(char *entrada, struct proc_dir_entry *parent);

