/*
 *  hello6_init.c 
 *      Dividir un modulo en varios archivos fuente
 */

#include <linux/module.h>	//modulos

int numero_ejercicio = 6 ;
int modulo_aiso_init(void)
{
	printk(KERN_INFO "Hola %d \n", numero_ejercicio);
	return 0;
}

/* declaracion <init> */
module_init(modulo_aiso_init);
