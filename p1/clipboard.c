#include "clipboard.h"

/* Variables globales */
extern struct list_head lista_clipboards;
extern unsigned int elemento_actual;
unsigned long caracteres_copiar; //buffer_size

/* ----------------------------------------------------------- */

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
    int terminado;
    printk(KERN_INFO "leer_clipboard. Seleccionado: %d\n", elemento_actual);
    
    /* determinar si hemos terminado de escribir */
    if (offset > 0) {
        terminado = 0;
    } else {
        /* copiar el contendido del buffer del clipboard en el buffer del sistema */   
        struct clipstruct *seleccionado = encontrar_clipboard();    
        memcpy(buffer, seleccionado->buffer, caracteres_copiar);
        terminado = caracteres_copiar;
    }
    
    return terminado;
}

/**
 * En buffer se almacena la entrada del usuario seguido de un salto de linea
 * Ejemplo: 34 => [3|4|\n|········]
 *  
 * En codigo ASCI 
 *
 * | char | codigo | 
 * |:----:|:------:|
 * | '0'  |  48    |
 * | '1'  |  49    |
 * | '2'  |  50    |
 * | '9'  |  57    |
 * | '\n' |  10    |
 *
 * @param buffer cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
extern int modificar_indice(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int nuevo_elemento;
    
    /* transformar 'c' en int */
    nuevo_elemento = atoi(buffer);
    
    /* Comprobar que nos han llamado con un id existente */
    // TODO
    if (true) {
        elemento_actual = nuevo_elemento;
        printk(KERN_INFO "Salimos de modificar_indice; elemento_actual = %d\n", elemento_actual);
    } else {
        printk(KERN_ALERT "modificar_indice => error");
    }
    return 4;  // tamaño de 1 byte 
}

/**
 * Funcion que se llama cuando escribimos en /proc : "echo"
 *
 * @param buffer del sistema cadena de entrada
 * @param count numero de caracteres a copiar
 * @return caracteres copiados
 */
extern int escribir_clipboard(struct file *file, const char *buffer, unsigned long count, void *data)
{
    struct clipstruct *seleccionado;
    printk(KERN_INFO "escribir_clipboard. Seleccionado: %d\n", elemento_actual);
    
    /* encontrar el buffer en el que vamos a escribir */
    seleccionado = encontrar_clipboard();
    printk(KERN_INFO "Encontrado la entrada con id: %d\n", seleccionado->id);
    
    /* copiar en el buffer seleccionado <= buffer */
    caracteres_copiar = count;
    if (caracteres_copiar > TAM_MAX_BUFFER) {
        caracteres_copiar = TAM_MAX_BUFFER;
    }

    if ( copy_from_user(seleccionado->buffer, buffer, caracteres_copiar) ) {
        return -EFAULT;
    }
    printk(KERN_INFO "Salimos de escribir_clipboard\n");
    
    return caracteres_copiar;
}

struct clipstruct* encontrar_clipboard(void)
{
    struct clipstruct *tmp = NULL;
    struct list_head *pos;

    list_for_each(pos, &lista_clipboards) {
        tmp = list_entry(pos, struct clipstruct, lista);
        if (tmp->id == elemento_actual){
            break;        
        }
    }    
    
    return tmp;
}

int atoi(const char* p)
{
    int numero_leido;
    int i = 0; 
    int numero = 0;
    
    // FIXME while( i < (sizeof p)/4 )
    while (1) {
      numero_leido = p[i];
      if (numero_leido == 10) {
        return numero_leido;
      } 
      numero_leido = numero_leido - 48;
      if (i == 0) {
        numero = numero_leido;
      } else {
        numero = numero * 10 + numero_leido;
      }
      i++;
    }
}
