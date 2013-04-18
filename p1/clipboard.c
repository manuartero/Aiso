#include "clipboard.h"

/**
 * Implementa las funciones  
 * -----------------------
 * leer_clipboard
 * escribir_clipboard
 * leer_indice
 * modificar_indice
 */

// ---------------------------------------------------------

/** 
 * Funcion que se llama cuando leemos del archivo /proc : "cat"
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
 */
extern int leer_indice(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO 
    return 0;
}

/** 
 * Funcion que se llama cuando leemos del archivo /proc : "cat"
 *
 * @param buffer donde copiaremos los datos
 * @param offset posicion actual del archivo
 * @return 0 si se ha acabado el archivo; valor negativo en caso de error
 */
extern int leer_clipboard(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO
    return 0;
}

/**
 * Funcion que se llama cuando escribimos en /proc : "echo"
 *
 * @param buffer cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
extern int modificar_indice(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // TODO
    return 0;
}

/**
 * Funcion que se llama cuando escribimos en /proc : "echo"
 *
 * @param buffer cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
extern int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data)
{
    /* encontrar el buffer en el que vamos a escribir */
    bool encontrado = false;    
    while (!encontrado)    

    /* buffer seleccionado <= buffer */

    return 0;
}

