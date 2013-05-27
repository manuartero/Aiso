#ifndef TESTER_H
#define TESTER_H

#include <stdio.h>      /* open - read */
#include <stdlib.h>     /* malloc - free */
#include <unistd.h>     /* struct stat 1 */
#include <sys/stat.h>   /* struct stat 2 */
#include <sys/types.h>  /* struct stat 3 */
#include <fcntl.h>      /* O_RDONLY */
#include <err.h>        /* perror */
#include <getopt.h>     /* getopt */

/* Declaracion de funciones */
static int copiar_fichero(char *fichero);
int main (int argc, char **argv);

#endif //TESTER_H
