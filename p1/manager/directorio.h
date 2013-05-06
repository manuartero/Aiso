#ifndef DIRECTORIO_H
#define DIRECTORIO_H
#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */
#include "../utiles/utiles.h"           /* utiles */




int crear_directorio_aiso(const char* nombre_directorio);

#endif /* DIRECTORIO_H */

