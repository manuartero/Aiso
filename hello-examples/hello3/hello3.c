/*  
 *  hello3.c
 *  ========
 *
 *  Declaracion static = private
 *
 *  Uso de macros 
 *      __init
 *      __initdata
 *      __exit
 */
#include <linux/module.h>   // modulos
#include <linux/init.h>		// macros 

/* Licencia del modulo */
MODULE_LICENSE("GPL");

/* macro __initdata, se liberara la memoria al finalizar el init */
static int numero_ejercicio __initdata = 3;

/* La macro _init hace que la funcion libere la memoria cuando termina la funcion de carga */
static int __init modulo_aiso_init(void)
{
	printk(KERN_INFO "Hola, ejercicio %d\n", numero_ejercicio);
	return 0;
	// Pensado para modulos que solo se cargaran una vez: una vez cargado y pasamos a ejecucion se libera la memoria
}

/* La macro __exit ¿? */
static void __exit modulo_aiso_exit(void)
{
	printk(KERN_INFO "Fin ejercicio %d\n", numero_ejercicio);
}

/* Declaracion de funciones <init> y <exit> */
module_init(modulo_aiso_init);
module_exit(modulo_aiso_exit);
