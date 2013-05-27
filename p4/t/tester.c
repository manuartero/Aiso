#include "tester.h"

/** Variables globales */
char * buffer;

/**
 * Lee el fichero especificado y lo almacena en un buffer
 */
static int copiar_fichero(char *fichero) 
{
  int fd_fichero;
  int tam_total;
  int tam;
  struct stat stat_fichero;

  // Abrir fichero
  fd_fichero = open(fichero, O_RDONLY);
  if (fd_fichero == -1){
    perror("error : open"),exit(-1);
  }

  // Consultar tam fichero en bytes   
  stat(fichero, &stat_fichero);
  tam_total = stat_fichero.st_size;

  // Crear un buffer de tam_total
  buffer = (char *) malloc(tam_total);
  if(buffer==NULL){
    perror ("error : malloc"),exit(-2);
  }

  // Leer el fichero: almacenar en buffer
  tam = read(fd_fichero, buffer, tam_total);
  if (tam==-1){
    perror("error : read"), exit(-3);
  }

  return 0;
}

/** --- */
int main (int argc, char **argv)
{
  int optc;
  
	// consumimos flags
  while ((optc = getopt (argc, argv, "h")) != -1){
    switch (optc){
			case 'h': //TODO 
				break;
			default : // TODO
				break;
		}
  }

	// optind contiene el numero de argumentos
  switch (argc - optind){
    case 0:	// $> test 
      // TODO
      break;

    case 1:	// $> test fichero
      copiar_fichero (argv[optind]);
      break;

    default:	
      warnx ("operandos incorectos\n");
      exit(-1);
   }

	// Liberamos el buffer
	if (buffer != NULL){
		free(buffer);
	}

	// TODO: cerrar el fichero

  exit(0);
}
