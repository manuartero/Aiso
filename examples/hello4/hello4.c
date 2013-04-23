/*  
 *  hello4.c
 *  ========
 *
 *  Quitar la declaracion de licencia del modulo
 *  $> [ 7696.035261] hello4: module license 'unspecified' taints kernel.
 *  $> [ 7696.035318] Disabling lock debugging due to kernel taint
 *
 *  Posibles licencias
 *  |----------|----------------------------------|
 *  | Publico  |  MODULE_LICENSE("GPL");          |
 *  | Privado  |  MODULE_LICENSE("Propietario");  |
 *  |---------------------------------------------|
 */
#include <linux/module.h>   // modulos
#include <linux/init.h>		// macros
 
int numero_ejercicio = 4;
int modulo_aiso_init(void)
{
	printk(KERN_INFO "Hola, ejercicio %d\n", numero_ejercicio);
	return 0;
}

void __exit modulo_aiso_exit(void)
{
	printk(KERN_INFO "Fin ejercicio %d\n", numero_ejercicio);
}

/* Declaracion de funciones <init> y <exit> */
module_init(modulo_aiso_init);
module_exit(modulo_aiso_exit);
