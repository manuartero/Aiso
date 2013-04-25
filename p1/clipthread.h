#ifndef CLIPTHREAD_H
#define CLIPTHREAD_H
#define __KERNEL_SYSCALLS__
 #include <asm/types.h>
#include <linux/signal.h>
#include <linux/module.h>	/* modulo */
#include <linux/kernel.h>	/* kernel */
#include <linux/kthread.h>





/* Declaracion de funciones */
int funcion_thread(void *data);
void tratamiento_signal(int signal);
pid_t mi_pid(void);
#endif /* CLIPTHREAD_H */

