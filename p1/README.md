Practica 1 - Clipboard múltiple
=========================


Funcionalidad
-----------------

#### Árbol de directorios

Al instalar el módulo se crean bajo la carpeta ```/proc/aisoclip``` tres ficheros virtuales:

 - **clipboard** : Permite leer y escribir cadenas de caracteres sobre los clipboards.
 - **selection** : Permite la selección de diferentes clipboards
 - **periodo** : Determina el periodo de activación del kernel-thread asociado

#### Implementación de los clipboards

Se ha empleado una lista doblemente enlazada de estructuras _clipstructus_

    struct clipstruct {
        unsigned int id;
        unsigned int num_elem;
        char* buffer;
        struct list_head lista;
    };

Existe un número máximo de clipboards que podemos crear en nuestro módulo. Por defecto este número es 5, pero es fácilmente ampliable mediante el parámetro de entrada ***num_clipboards***

    $> insmod clip_sin_manager num_clipboards=20.ko

Esto indicaría al módulo que nuestra lista contendrá hasta 20 nodos.   

> Nota: Las entradas de la lista no se crean al inicializar el módulo sino que se hace la reserva de memoria de manera dinámica **al escribir sobre la entrada selection**. De esta manera si hemos declarado un módulo de hasta 20 clipboards pero sólo usamos la mitad no estamos malgastando recursos.

#### Kernel thread asociado

El kernel thread se ha implementado evitando la espera activa. El proceso se duerme hasta que ocurren una de las siguientes opciones:

1. Entrada **periodo** activada : Se despierta de forma cíclica cada 10 segundos
2. El usuario escribe en clipboard o selection; se despierta al thread mediante la llamada al sistema ```wake_up_process(clipkthread)```

#### Otros aspectos relevantes

El módulo cuenta con las siguientes mejoras no especificadas en el enunciado:

 - Al pedir la selección o lectura de un clipboard inexistente avisa mediante un mensaje por consola
 - Al intentar reservar un clip más allá del límite de clipboards establecidos avisa mediante un mensaje por consola
 - Mensajes de error específicos de consola
 - Se puede leer de la entrada periodo 
 - Se puede leer de la entrada indice 

***

Instalación
-----------

1. Compilar el archivo ```clipboard.c``` mediante la orden make
2. Instalar el módulo: ```$> insmod clip_sin_manager.ko```

***

Ejemplo de uso
--------------------

    echo Esto es un ejemplo > /proc/aisoclip/clipboard
    cat /proc/aisoclip/clipboard

=> Esto es un ejemplo

    echo 4 > /proc/aisoclip/selection
    echo Esto lo guardo en el cuatro > /proc/aisoclip/clipboard
    cat /proc/aisoclip/clipboard

=> Esto lo guardo en el cuatro

    echo 1 > /proc/aisoclip/selection
    cat /proc/aisoclip/clipboard

=> Esto es un ejemplo
