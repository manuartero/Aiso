#ifndef CLIPTHREAD_H
#define CLIPTHREAD_H

#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/kthread.h>  /* thread */
#include "clipstruct.h"     /* clipstruct */

/* Declaracion de funciones */
int funcion_thread(void *data);

#endif /* CLIPTHREAD_H */

