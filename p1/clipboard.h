#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include <linux/list.h>     /* listas del kernel */
#include <asm/uaccess.h>    /* function copy_from_user */
#include "clipstruct.h"     /* struct clipstruct */

extern int leer_indice(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
extern int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
extern int modificar_indice(struct file *file, const char *buffer, unsigned long count, void *data);
extern int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data);

#endif /* CLIPBOARD_H */

