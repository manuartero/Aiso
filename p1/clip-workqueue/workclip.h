#ifndef WORKQUEUE_H
#define WORKQUEUE_H
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/vmalloc.h>          /* funcion vmalloc */

struct work_struct_ampliado {
  struct work_struct work;
  char  mensaje[256];
};
   
/* Funciones */
void procesar_nodo(struct work_struct * nodo);
int encolar_tarea(struct workqueue_struct * cola, char * mensaje);

#endif /* WORKQUEUE_H */

