/*  
 *  hello2.c
 *  ========
 *
 *  Modulo basico con valor de return negativo
 * 
 *  El mensaje de error cambia en funcion del numero.
 *  El pintk de la funcion init se muestra
 *  No se puede desistalar el modulo: ERROR: Module hello2 does not exist in /proc/modules
 */
#include <linux/module.h> // modulos

/* Licencia del modulo */
MODULE_LICENSE("GPL");

/* Funcion que se invca cuando se carga el modulo en  el kernel*/
int modulo_aiso_init(void)
{
  printk(KERN_INFO "HOLA HOLA modulo ejercicio 2\n");
  
  return -3; // insmod: error inserting 'hello2.ko': -1 Module has wrong symbol version
}

/* Funcion que se invoca cuando se descarga el modulo */
void modulo_aiso_clean(void)
{
  printk(KERN_INFO "ADIOS\n");
  return;
}

// Declaracion de funciones <init> y <exit>
module_init(modulo_aiso_init);
module_exit(modulo_aiso_clean);
