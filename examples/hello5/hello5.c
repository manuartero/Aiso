/*
 *  hello5.c
 *
 * Paso de argumentos a un modulo
 *
 * Macros
 * =======
 *  [1] module_param(nombre_parametro, tipo, permisos)
 *  [2] module_param_array(nombre_array, tipo, puntero_contador_elementos, permisos)
 *
 * Ejecucion
 * =========
 *   ./insmod mymodule.ko myvariable=5
 *   ./insmod mymodule.ko myintArray=4,7
 */

#include <linux/module.h>       // modulos
#include <linux/moduleparam.h>  // paso de parametros
#include <linux/init.h>         // macros
#include <linux/stat.h>         //¿permisos?

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");

static short int myshort;
static int myint;
static long int mylong;
static char *mystring;
static int arr_argc = 0; // puntero al numero de elementos, EMPEZAR EN 0
static int myintArray[2];


// short
module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");

// int
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");

// long
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");

// String
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");

// Array
module_param_array(myintArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintArray, "An array of integers");

static int __init modulo_aiso_init(void)
{
	printk(KERN_INFO "Hello, world 5\n=============\n");
	printk(KERN_INFO "myshort is a short integer: %hd\n", myshort);
	printk(KERN_INFO "myint is an integer: %d\n", myint);
	printk(KERN_INFO "mylong is a long integer: %ld\n", mylong);
	printk(KERN_INFO "mystring is a string: %s\n", mystring);
	
	int i;
	for (i = 0; i < (sizeof myintArray / sizeof (int)); i++)
	{
		printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]);
	}
	printk(KERN_INFO "got %d arguments for myintArray.\n", arr_argc);
	
	return 0;
}

static void __exit modulo_aiso_exit(void)
{
	printk(KERN_INFO "Adios 5\n");
}

/* Declaracion de funcinoes <init> <exit> */
module_init(modulo_aiso_init);
module_exit(modulo_aiso_exit);
