#ifndef TESTER_H
#define TESTER_H

#include <stdio.h>              /* printf */
#include <stdlib.h>             /* malloc */
#include <unistd.h>             /* exit */
#include <fcntl.h>              /* open */
#include <sys/stat.h>           /* stat */
#include <sys/ioctl.h>          /* ioctl */
#include <err.h>                /* perror */
#include <getopt.h>             /* getopt */
#include "../funciones_ioctl.h" /* funciones ioctl */
#include <string.h>

/* Declaracion de funciones */
static int leer_fichero(int count);            
static inline void reset_buffer(void);    
static inline void escribir_fichero(char * texto);
static inline void modificar_buffer(int cantidad);
static inline void lseek_fichero(int posicion,int modo);
static void mostrar_ayuda(void);
static inline void consulta(void);

int main (int argc, char **argv);

#endif //TESTER_H

