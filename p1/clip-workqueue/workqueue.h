#ifndef WORKQUEUE_H
#define WORKQUEUE_H
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

struct work_struct_ampliado {
  struct work_struct work;
  char * mensaje;
};
 
/* Funciones */
void procesar_nodo(struct work_struct * nodo);
int encolar_tarea(struct work_struct_ampliado * cola, const char * mensaje);

#endif /* WORKQUEUE_H */

