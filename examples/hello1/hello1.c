/*  
 *  hello1.c
 *  ========
 *
 *  Modulo basico.
 *
 */
#include <linux/module.h> // modulos

/* Licencia del modulo */
MODULE_LICENSE("GPL");

/* Funcion que se invca cuando se carga el modulo en  el kernel*/
int modulo_aiso_init(void)
{
  printk(KERN_INFO "HOLA HOLA \n");
  
  return 0;
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
