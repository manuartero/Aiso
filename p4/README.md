Práctica 4 - Driver de caracteres
=================================

Índice
------

1) [Funcionalidad] (#Funcionalidad)
2) [Instalación] (#Instalación)
3) [Uso] (#Uso)

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

 - POR HACER

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

POR HACER

### Ejemplo de uso del programa tester_app

POR HACER


