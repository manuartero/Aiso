Práctica 4 - Driver de caracteres
=================================

Índice
------

1) [Funcionalidad] (#funcionalidad)  
2) [Instalación] (#instalacin)  
3) [Uso] (#uso)  

***

Funcionalidad
--------------

Driver de caracteres que define las siguientes operaciones

 1) Apertura del fichero ```/dev/aiso_device```  
 2) Escritura y lectura  
 3) Posicionamiento del puntero de lectura (mediante posición absoluta o relativa)  
 4) Reset del buffer del fichero (IOCTL)  
 5) Cambio del tamaño del buffer de almacenamiento (IOCTL)  
 
La estructura ```file_operations``` que define las operaciones definidas es la siguiente:

```
static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = aiso_read,
	.write = aiso_write,
	.open = aiso_open,
	.release = aiso_release,
    .ioctl = aiso_ioctl
};
```

Cuenta con un kernel thread asociado que cuenta la cantidad de veces que el fichero ha sido abierto e imprime el valor en el log del kernel.

El registro del número mayor se hace de forma dinámica mediante la función ```int alloc_chrdev_region (dev_t * dev, unsigned baseminor, unsigned count, const char * name); ```
 
### Otros aspectos relevantes

 - El kernel thread se ha implementado evitando la espera activa. El proceso se duerme hasta que es despertado en la llamada de la función ___open___ del fichero.

 - Además de las funciones pedidas se han añadido dos opciones más a *IOCTL* : ***IOCTL_WRITTEN*** e ***IOCTL_POINTER*** que nos aportan información sobre el fichero

 - *IOCTL_WRITTEN* Devuelve el número de caracteres escritos en el fichero

 - *IOCTL_POINTER* Devuelve la posición actual del puntero de lectura

 - El script de instalación del driver tiene en cuenta si es necesario desistalar un módulo previo con el mismo nombre que el que se intenta instalar

Instalación
------------

1) Compilar el driver

``` /Aiso/p4 $> make ```

2) Ejecutar el script de instalación (será necesario ser super usuario)

``` /Aiso/p4 #> ./install.sh ```

Uso
----

Junto con el código del driver se entrega una aplicación de testeo.  
Se encuentra en la carpeta *tester_app*. Una vez en la carpeta de la aplicación compilamos para obtener el ejecutable:

``` /Aiso/p4/tester_app $> make ```

### Sobre el programa de pruebas

El programa de testeo está implementado en C.
Permite hacer uso de todas las funciones implementeadas en el driver y ver los resultados de forma clara y directa.

El programa dispone de los siguientes flags

| Flag    | Llamada                | Funcionalidad          |
|:-------:|:-----------------------|:-----------------------|
| Ninguno | ./tester_app           | Muestra los caracteres escritos y la posición de la cabecera |
| r       | ./tester_app -r {num}  | Lee el fichero {num} posiciones desde la cabecera |
| w       | ./tester_app -w {cadena} | Escribe la {cadena} de entrada en el fichero |
| s       | ./tester_app -s        | Posiciona la cabecera de lectura al principio del fichero |
| s       | ./tester_app -s {num}  | Posiciona la cabecera de lectura en la posición absoluta indicada |
| c       | ./tester_app -c {num}  | Posiciona la cabecera de lectura en la posición relativa indicada |
| e       | ./tester_app -e        | Posiciona la cabecera de lectura al final del fichero |
| m       | ./tester_app -m {num}  | Cambia el tamaño del buffer por la cantidad indicada |
| R       | ./tester_app -R        | Borra el contenido del buffer y posiciona la cabeza al principio |
| h       | ./tester_app -h        | Muestra la ayuda del programa |

> Nota: el programa abre el archivo *open* al inicio de la función ___main___

### Ejemplo de uso del programa tester_app

A continucación se muestra un ejemplo sencillo de uso del programa

``` 
/Aiso/p4/tester_app $> ./tester_app  
> caracteres escritos: 0  
> posicion de la cabeza: 0
  
/Aiso/p4/tester_app $> ./tester_app -w "hola hola"  

/Aiso/p4/tester_app $> ./tester_app  
> caracteres escritos: 9  
> posicion de la cabeza: 9

/Aiso/p4/tester_app $> ./tester_app -s

/Aiso/p4/tester_app $> ./tester_app -r 4  
hola

/Aiso/p4/tester_app $> ./tester_app -R

/Aiso/p4/tester_app $> ./tester_app  
> caracteres escritos: 0  
> posicion de la cabeza: 0  
```

***

