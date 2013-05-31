#include "tester.h"

/** Variables globales */
char * buffer;

/** Flags */
int read_mode =   0;
int write_mode =  0;
int lseek_mode =  0;
int reset_mode =  0;
int modify_mode = 0;

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
    while ((optc = getopt (argc, argv, "rwlRmh")) != -1){
        switch (optc){
			case 'r': // READ
                read_mode = 1; 
				break;

            case 'w': // WRITE
                write_mode = 1;
                break;

            case 'l': // LSEEK
                lseek_mode = 1;
                break;

            case 'R': // RESET
                reset_mode = 1;
                break;

            case 'm': // MODIFY
                modify_mode = 1;
                break;

            case 'h': // HELP
                mostrar_ayuda();
                break;

			default : // HELP
                mostrar_ayuda();
				break;
		}
    }

	// optind contiene el numero de argumentos
  switch (argc - optind){
    case 0:	// $> test 
      // TODO
      break;

    case 1:	// $> test opc
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
