#include "tester.h"

/** Variables globales */
char * buffer;
int fd_fichero;

/** Flags */
int read_mode =   0;
int write_mode =  0;
int lseek_mode =  0;
int reset_mode =  0;
int modify_mode = 0;

/** Programa de testeo:
 * 
 * 1) Abre el fichero definido en funciones_ioctl.h y reserva un buffer
 * 2) Consume los flags de entrada rwlRmh
 * 3) Espera como entrada las siguientes opciones
 *      ./app -r
 *      ./app -R
 *      ./app -h
 *      ./app -l
 *      ./app -l numero
 *      ./app -w cadena
 *      ./app -m numero
 * 4) cierra el fichero
 */
int main (int argc, char **argv)
{
    int optc;
    struct stat stat_fichero;
    int tam_fichero;
    
    // abrimos el fichero
    fd_fichero = open(RUTA_FICHERO, O_RDONLY);
    if (fd_fichero == -1){
        perror("error : inicio, abrir fichero"),exit(-1);
    }

    // Consultar tam fichero en bytes   
    stat(RUTA_FICHERO, &stat_fichero);
    tam_fichero = stat_fichero.st_size;

    // Crear un buffer de tam_fichero
    buffer = (char *) malloc(tam_fichero);
    if(buffer==NULL){
        perror ("error : inicio, malloc"),exit(-2);
    }

    // Consumimos flags
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

			default : // READ
                read_mode = 1;
				break;
		}
    }

    // optind contiene el numero de argumentos no flags
    switch (argc - optind){
        case 0:	// $> test [opciones] 
            if(read_mode){
                leer_fichero();            
            } else if(reset_mode) {
                reset_buffer();    
            } else if(lseek_mode){
                lseek_fichero(0);
            } else {
                warnx("opcion incorrecta \n");
                exit(-3);
            }
            break;

        case 1:	// $> test [opciones] argumento
            if(write_mode){
                escribir_fichero(argv[2]);
            } else if (modify_mode) {
                modificar_buffer((int) argv[2]);
            } else if (lseek_mode) {
                lseek_fichero((int) argv[2]);
            } else {
                warnx("opcion incorrecta \n");
                exit(-4);
            };
            break;

        default:	
            warnx ("operandos incorectos\n");
            exit(-5);
   }

	// Liberamos el buffer
	free(buffer);
	
	// Cerrar el fichero
    close(fd_fichero);

    // Salir de la aplicacion
    exit(0);
}

/** Funciones */

static void mostrar_ayuda(void)
{
    printf("uso: [OPCIONES] [ARGUMENTO]\n");
    printf("./app : leer el fichero\n");
    printf("./app -r : Leer el fichero\n");
    printf("./app -R : Reset el buffer\n");
    printf("./app -l : Posiciona la cabeza al incio\n");
    printf("./app -l {numero} : Posiciona la cabeza a la posicion indicada\n");
    printf("./app -w {cadena} : Escribe la cadeana en el fichero\n");
    printf("./app -m {numero} : Modifica el buffer al tam indicado \n");   
}
    
static void leer_fichero(void)
{
    int respuesta;
    printf("Lectura\n"); 

	respuesta = ioctl(fd_fichero, IOCTL_READ, &buffer);
	
    if(respuesta < 0){
		printf("error : ejecucion, lectura fichero buffer=>%s\n", buffer);
        exit(-7);
	}
	
    printf("FIN Lectura\n");
}           

static void reset_buffer(void)
{
    int respuesta;
    printf("Reset\n");

    respuesta = ioctl(fd_fichero, IOCTL_RESET, NULL);

    if(respuesta<0){
        printf("error : ejecucion, reset \n");
        exit(-9);
    }

    printf("FIN Reset\n");
}    

static void lseek_fichero(int posicion)
{
    int respuesta;
    printf("Lseek\n");

	respuesta = ioctl(fd_fichero, IOCTL_LSEEK, posicion);
	
    if(respuesta < 0){
		printf("error : ejecucion, Lseek posicion=>%d\n", posicion);
        exit(-8);
	}

	printf("FIN Lseek\n");
}

static void escribir_fichero(char * texto)
{
    int respuesta;
    printf("Escritura\n");    
    
    respuesta = ioctl(fd_fichero, IOCTL_WRITE, texto);
	
    if(respuesta < 0){
		printf("error : ejecucion, escribir fichero texto=>%s\n", texto);
        exit(-6);
	}

	printf("FIN Escritura\n");
}

static void modificar_buffer(int cantidad)
{
    int respuesta;
    printf("Modificar\n");    
    
    respuesta = ioctl(fd_fichero, IOCTL_MODIFY, cantidad);
	
    if(respuesta < 0){
		printf("error : ejecucion, modificar cantidad=>%d\n", cantidad);
        exit(-10);
	}

	printf("FIN Modificar\n");
}

/*
static int copiar_fichero(char *fichero) 
{
 // Leer el fichero: almacenar en buffer
  tam = read(fd_fichero, buffer, tam_total);
  if (tam==-1){
    perror("error : read"), exit(-3);
  }

  return 0;
}
*/

