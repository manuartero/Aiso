Práctica 2 - Clipboard con manager
===============================


Funcionalidad
-----------------

#### Árbol de directorios

Al instalar el módulo se crean bajo la carpeta ```/proc/aisoclip``` tres ficheros virtuales:

 - **activar** : Permite instalar nuevos clipboards bajo el directorio ```/proc/aisoclip```
 - **desactivar** : Permite desistalar un clipboard instalado.
 - **monitor** : Aporta información sobre los clipboards activos.

#### Instalación de módulos desde el manager

Al escribir sobre la entrada **activar** se crea un nuevo clipboard con el nombre especificado.  
Si por ejemplo instalamos el módulo _hola_ tendríamos la siguiente jerarquía

    /proc/aisoclip
        activar
        desactivar
        hola/
            clipboard
            periodo
            selection
        monitor

> Nota : La instalación de módulos se hace mediante *modprobe*
> La desistalación de módulos se hace mediante *rmmod*

#### Implementación de los clipboards

Se han reutilizado los [clipboard implementados para la primera parte de la práctica] (https://github.com/manutero/Aiso/tree/master/p1#implementacin-de-los-clipboards)

Gracias a esto, los clipboard instalados desde el manager cuentan, a su vez, con una lista doblemente enlazada de estructuras _clipstructus_ en la que se reserva memoria de forma dinámica y el [kernel-thread asociado] (https://github.com/manutero/Aiso/tree/master/p1#kernel-thread-asociado)

> Nota : La única diferencia con respecto a los clipboard de la primera parte es que se ha eliminado la posibilidad de ampliar el número de elementos en la lista de forma que queda restringido a un máximo de 5 elementos.

#### Otros aspectos relevantes

El módulo cuenta con las siguientes mejoras no especificadas en el enunciado:

 - No es necesario añadir el flag ___-n___ a la orden _echo_ al  escribir el nombre de los clipboard.
 - Se avisa mediante consola al intentar instalar un módulo ya cargado.
 - Se avisa mediante consola al intentar borrar un módulo no existente
 - Al separar en carpetas independientes los archivos fuente, requiere de una instalación más elaborada.

***

Instalación
-----------

1. Compilar el archivo ```manager.c``` mediante la orden ```make```
2. Copiar el archivo ```Module.symvers``` generado a la carpeta ```clip-thread```
3. Compilar el clip : 
    $> cd ../clip-thread ; make 
4. Copiar el ```clip1.ko``` obtenido a la carpeta ```/lib/modules/<version-kernel>```
5. Ejectuar ```$> depmod -a```
6. Instalar ```p1.ko``` (archivo en la carpeta manager)

***

Ejemplo de Uso
---

     echo hola > /proc/aisoclip/activar
     echo adios > /proc/aisoclip/activar
     echo adios > /proc/aisclip/desactivar
     cat /proc/aisoclip/monitor

=> Activo: hola

    echo ejemplo > /proc/aisoclip/hola/clipboard
    cat /proc/aisoclip/hola/clipboard

=> ejemplo

    echo 3 > /proc/aisoclip/hola/selection
    echo ejemplo2 > /proc/aisoclip/hola/clipboard
    cat /proc/aisoclip/hola/clipboard

=> ejemplo2


