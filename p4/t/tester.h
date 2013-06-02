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

/* Declaracion de funciones */
static void leer_fichero(void);            
static void reset_buffer(void);    
static void escribir_fichero(char * texto);
static void modificar_buffer(int cantidad);
static void lseek_fichero(int posicion);
static void mostrar_ayuda(void);

int main (int argc, char **argv);

#endif //TESTER_H

