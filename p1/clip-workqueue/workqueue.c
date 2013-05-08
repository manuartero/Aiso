#include "clipboard.h"

/**
 * Funcion de callback al procesar un nodo 
 * @param nodo a procesar 
 */
void procesar_nodo(struct work_struct * nodo)
{
    // 1) Hacemos el casting a nuestra definicion de estructura
    nodo_cola * n = (nodo_cola *) nodo;
    
    // 2) Procesamos el mensaje
    printk(KERN_INFO "Procesado del nodo %d ==> %s\n", n->id, n->mensaje );

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
int encolar_tarea(struct work_struct_ampliado * cola, const char * mensaje)
{
    int error = 0;
    
    // 1) Reservamos memoria para un nuevo nodo
    work_struct_ampliado * nodo = (work_struct_ampliado *) kmalloc( sizeof(work_struct_ampliado), GFP_KERNEL);
    
    if (nodo) {
      // 2) Registramos la funcion
      INIT_WORK( (struct work_struct *) work, procesar_nodo );
      // 3) Rellenamos la estructura
      s = vmalloc(sizeof(mensaje));
      strcpy(nodo->mensaje, mensaje);
      // 4) Encolamos el nodo
      error = queue_work(cola, (struct work_struct_ampliado *) nodo);
    }
    
    return error;
}

