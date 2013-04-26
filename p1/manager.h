#ifndef MAIN_H
#define MAIN_H

#include <linux/module.h>	        /* modulo */
#include <linux/kernel.h>	        /* kernel */
#include <linux/proc_fs.h>	        /* struct proc_dir_entry */
#include <asm/uaccess.h>            /* function copy_from_user */
#include <linux/moduleparam.h>      /* paso de parametros */
#include "clipboard.h"              /* clipboard */

// funiones basicas
int modulo_p1_init(void);

void modulo_p1_clean(void);

// funciones de lectura
int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

// funciones de escritura
int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data);

