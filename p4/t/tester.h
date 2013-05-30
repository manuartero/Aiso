#ifndef TESTER_H
#define TESTER_H

#include <stdlib.h>             /* malloc */
#include <unistd.h>             /* exit */
#include <fcntl.h>              /* open */
#include <sys/ioctl.h>          /* ioctl */
#include <err.h>                /* perror */
#include <getopt.h>             /* getopt */
#include "../funciones_ioctl"   /* funciones ioctl */

/* Declaracion de funciones */
static int copiar_fichero(char *fichero);
int main (int argc, char **argv);

#endif //TESTER_H
