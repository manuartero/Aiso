/*
 *  hello6_exit.c 
 *      Dividir un modulo en varios archivos fuente
 */

#include <linux/module.h>	// modulos
extern int numero_ejercicio;

void modulo_aiso_exit(void)
{
	printk(KERN_INFO "Adios %d \n", numero_ejercicio);
}

/* funcion <exit> */
module_exit(modulo_aiso_exit);
