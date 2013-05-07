#include "clipboard.h"

my_work_t *work, *work2;

void procesar_nodo(struct work_struct * nodo)
{
    // 1) Hacemos el casting a nuestra definicion de estructura
    nodo_cola * n = (nodo_cola *) nodo;
    
    // 2) Procesamos el mensaje
    printk( "Procesado del nodo %d : %s\n", n->id, n->mensaje );

    // 3) Liberamos los recursos
    vfree( (char *) n->mensaje);
    kfree( (void *) work );    
    return;
}

/** 
 * @param (struct nodo_cola *) en esta estructura
 * @param (const char *) encola un nodo con este mensaje
 * @return (int) flag errores
 */
int encolar_tarea(struct (nodo_cola *) cola, const char * mensaje)
{
    int error = 0;
    nodo_cola * work = (nodo_cola *) kmalloc(sizeof(nodo_cola), GFP_KERNEL);
    
    if (work) {
      INIT_WORK( (struct work_struct *) work, procesar_nodo );
      s = vmalloc(sizeof(mensaje));
      strcpy(nodo->mensaje, mensaje);
      error = queue_work( cola, (struct work_struct *)work );
    }
    
    return error;
}

