#include "clipstruct.h"     /* struct clipstruct */
#include "clipboard.h"      /* funciones del clipboard */
#include "clipthread.h" 	/* funciones para el thread */

#include <linux/moduleparam.h>  // paso de parametros
#include <linux/init.h>         // macros
#include <linux/stat.h>         //¿permisos?

#define nombre_directorio "aisoclip"
#define nombre_entrada "clipboard"
#define nombre_selector "selection"
#define nombre_periodo "periodo"

/* Declaracion de funciones */
int modulo_aiso_init(void);
void modulo_aiso_clean(void);

