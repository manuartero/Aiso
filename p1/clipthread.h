#ifndef CLIPTHREAD_H
#define CLIPTHREAD_H
#include <linux/signal.h>
#include <linux/kthread.h>
#define _XOPEN_SOURCE

/* Declaracion de funciones */
int funcion_thread(void *data);
void tratamiento_signal(int signal);

#endif /* CLIPTHREAD_H */

