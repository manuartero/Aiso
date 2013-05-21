Practica 1 - Clipboard múltiple
===============================


Funcionalidad
-------------

#### 1) clipboard basico

 - Se interactua mediante ficheros virtuales en el directorio /proc/aisoclip 
 - tendremos NCLIP clipboards a los que se accedera via /proc/aisoclip/clipboard.
 - se eligira el clipboard mediante /proc/aisoclip/selection.
 - NCLIP se fijara mediante un parametro. 
 
> el clipboard multiple se implementara con una lista doblemente enlazada de estructuras __clipstructs__

#### 2) Thread

Descripción por hacer

#### 3) manager

Descripción por hacer

#### 4) worqueue

Descripción por hacerse

***

Instalación
-----------

1. compilar el manager 
2. copiar el archivo ```Module.symvers``` a la carpeta ```clip-thread``` o ```clip-worqueue```
3. compilar el clip que vayamos a usar 
4. copiar el ```.ko``` obtenido a la carpeta ```/lib/modules/<version-kernel>```
5. ejectuar ```$> depmod -a```
6. instalar ```p1.ko``` (archivo en la carpeta manager)

***

Uso
---

POR HACER

