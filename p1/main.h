#include "clipstruct.h"     /* struct clipstruct */
#include "clipboard.h"      /* funciones lectura y escritura */
#include <linux/moduleparam.h>  // paso de parametros
#include <linux/init.h>         // macros
#include <linux/stat.h>         //¿permisos?
#include <linux/signal.h>
#include <linux/kthread.h>


/* Declaracion de funciones */
int modulo_aiso_init(void);
void modulo_aiso_clean(void);
int escritura_thread(void *data); 

