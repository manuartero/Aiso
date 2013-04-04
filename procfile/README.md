PROC1.c
-------

Ejemplo basico de uso de ```/proc``` 
 * Crear un archivo en ```/proc/aiso``` en la funcion __init_module__
 * Copiar una cadena en el buffer en la llamada __write__
 * Volvar el contentido del buffer en la llamada __read__
 * Borrar el archivo ```/proc/aiso``` en la function __cleanup_module__
