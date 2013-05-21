Practica 3 - Clipboard con workqueues
==============================


Funcionalidad
-----------------

#### Funcionalidad básica

La funcionalidad de la práctica es exactamente la misma que para la [versión 2] (https://github.com/manutero/Aiso/blob/master/p2/README.md#funcionalidad).

La diferencia está en el uso de una estructura *workqueue_struct* para mostrar mensajes *KERN_INFO*

La estructura utilizada está declarada en el archivo ```workclip.h```
    struct work_struct_ampliado {
        struct work_struct work;
        char  mensaje[256];
    };

#### Kernel thread asociado

El kernel thread se ha implementado evitando la espera activa. En esta versión en lugar de lanzar la orden ```printk()``` encola una nueva tarea en la *workqueue* y la cola sacará en el log del kernel el mensaje correspondiente.

#### Otros aspectos relevantes

El módulo cuenta con las siguientes mejoras no especificadas en el enunciado:

 - Se aprovecha la cola de tareas para encolar todos los mensajes de información del módulo.
 - Al separar en carpetas independientes los archivos fuente, requiere de una instalación más elaborada.
 - Todas las entradas son manejables sin necesidad de ser super usuario
 
***

Instalación
-----------

1. Compilar el archivo ```manager.c``` mediante la orden ```make```
2. Copiar el archivo Module.symvers generado a la carpeta ```clip-workqueue```
3. Compilar el clip : ```$> cd ../clip-workqueue ; make```
4. Copiar el ```clip2.ko``` obtenido a la carpeta /lib/modules/<version-kernel>
4. Ejectuar ```$> depmod -a```
5. Instalar ```p1.ko``` (archivo en la carpeta manager)

***

Ejemplo de uso
--------------------

Ver el [ejemplo de uso para la parte 2] (https://github.com/manutero/Aiso/blob/master/p2/README.md#ejemplo-de-uso)
