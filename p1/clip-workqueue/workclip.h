#ifndef WORKQUEUE_H
#define WORKQUEUE_H
 
#include <linux/kernel.h>           /* kernel */
#include <linux/module.h>           /* module */
#include <linux/workqueue.h>        /* workqueue_struct */
#include <linux/vmalloc.h>          /* funcion vmalloc */

struct work_struct_ampliado {
  struct work_struct work;
  char  mensaje[256];
};
   
/* Funciones */
void procesar_nodo(struct work_struct * nodo);
int encolar_tarea(struct workqueue_struct * cola, char * mensaje);

#endif /* WORKQUEUE_H */

