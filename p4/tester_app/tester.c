#include "tester.h"

/** Variables globales */
char * buffer;
int fd_fichero;
int posicion;

/** Flags */
int read_mode;
int write_mode;
int lseek_set;
int lseek_curr;
int lseek_end;
int reset_mode;
int modify_mode;
int no_flag;

void inline limpiar_flags(void)
{
    read_mode =   0;
    write_mode =  0;
    lseek_set =  0;
    lseek_curr = 0;
    lseek_end = 0;
    reset_mode =  0;
    modify_mode = 0;
    no_flag = 0;
}

/** Programa de testeo:
 * 
 * 1) Abre el fichero definido en funciones_ioctl.h y reserva un buffer
 * 2) Consume los flags de entrada rwlRmh
 * 3) Espera como entrada las siguientes opciones
 *      ./app -r
 *      ./app -R
 *      ./app -h
 *      ./app -s {numero}
 *		./app -c {numero}
 *		./app -e {numero}
 *      ./app -w cadena
 *      ./app -m numero
 * 4) cierra el fichero
 */
int main (int argc, char **argv)
{
    int optc;
    struct stat stat_fichero;
    int tam_fichero;
    limpiar_flags();

    // abrimos el fichero
    fd_fichero = open(RUTA_FICHERO, O_RDWR);
    if (fd_fichero == -1){
        perror("error : inicio, abrir fichero"),exit(-1);
    }

    // Consultar tam fichero en bytes   
    stat(RUTA_FICHERO, &stat_fichero);
    tam_fichero = stat_fichero.st_size;
    posicion = 0;

    // Crear un buffer de tam_fichero 
    buffer = (char *) malloc(tam_fichero);
    if(buffer==NULL){
        perror ("error : inicio, malloc"),exit(-2);
    }
    
    if(argc == 1){
        no_flag = 1;
    }

    // Consumimos flags
    while ((optc = getopt (argc, argv, "rwsceRmh")) != -1){
        switch (optc){
			case 'r': // READ
                read_mode = 1;
				break;

            case 'w': // WRITE
                write_mode = 1;
                break;

            case 's': // LSEEK seek_set
                lseek_set = 1;
                break;
			
			case 'c': // lSEEK seek_curr
				lseek_curr = 1;
				break;
			
			case 'e': //LSEEK seek_end
				lseek_end = 1;
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

			default : // STAT
                no_flag = 1;
				break;
		}
    }

    // optind contiene el numero de argumentos no flags
    switch (argc - optind){
        case 0:	// $> test [opciones] 
            if(no_flag){
                consulta();
            }else if(reset_mode) {
                reset_buffer();    
            } else if(lseek_set){
                lseek_fichero(0,0);
            } else if(lseek_curr){
                lseek_fichero(0,1);
            }else if(lseek_end){
                lseek_fichero(0,2);
            }else {
                warnx("opcion incorrecta \n");
                exit(-3);
            }
            break;

        case 1:	// $> test [opciones] argumento
            if(write_mode){
                escribir_fichero(argv[2]);
            } else if (modify_mode) {
                modificar_buffer((int) atoi(argv[2]));
            }else if(read_mode){
                leer_fichero(atoi(argv[2]));            
            } else if (lseek_set) {
                lseek_fichero((int) atoi(argv[2]),0);
            }else if (lseek_curr) {
                lseek_fichero((int) atoi(argv[2]),1);
            }else if (lseek_end) {
                lseek_fichero((int) atoi(argv[2]),2);
            }else {
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
    printf("./tester_app : consulta el numero de caracteres escritos \n");
    printf("./tester_app -r :{numero} Leer del fichero un numero de caracteres\n");
    printf("./tester_app -R : Reset el buffer\n");
   // printf("./tester_app -l : Posiciona la cabeza al incio\n");
    printf("./tester_app -s {numero} : Posiciona la cabeza a la posicion indicada\n");
    printf("./tester_app -c {numero} : Posiciona la cabeza a la posicion actual mas la posicion indicada\n");
    printf("./tester_app -e {numero} : Posiciona la cabeza al final mas la posicion indicada\n");
    printf("./tester_app -w {cadena} : Escribe la cadeana en el fichero\n");
    printf("./tester_app -m {numero} : Modifica el buffer al tam indicado \n");   
    printf("------------------------------------------------------------- \n");
}

/**
 * Imprime el numero de caracteres escritos en el fichero
 */    
static inline void consulta(void)
{
    int bytes_escritos = 0;
    ioctl(fd_fichero, IOCTL_WRITTEN, &bytes_escritos);
    ioctl(fd_fichero, IOCTL_POINTER, &posicion);
    printf("Escrito : %d\n", bytes_escritos);
    printf("Puntero : %d\n", posicion);
}

/** 
 * Imprime el fichero
 */
static int leer_fichero(int count)
{
    int respuesta = 0;   
    
    respuesta = read(fd_fichero,buffer,count);
    
    if(respuesta < 0){
		printf("error : ejecucion, lectura fichero \n");
        exit(-7);
	}	

    printf("%s\n", buffer);
    return respuesta;
}           

static inline void reset_buffer(void)
{
    int respuesta;
    
    respuesta = ioctl(fd_fichero, IOCTL_RESET, NULL);
    posicion = 0;

    if(respuesta<0){
        printf("error : ejecucion, reset \n");
        exit(-9);
    }

    return;
}    

static inline void lseek_fichero(int nueva_posicion,int modo)
{
	int respuesta = 0;
   
   	respuesta = lseek(fd_fichero,nueva_posicion,modo);
		
    if(respuesta < 0){
		printf("error : ejecucion, Lseek posicion=>%d \n", posicion);
        exit(-8);
	}
	
    printf("nueva_posicion %d\n",respuesta);
    return;	
}

/**
 * Escribe el texto de entrada en el fichero
 */
static inline void escribir_fichero(char * texto)
{
    int respuesta;
  
    respuesta = write(fd_fichero,texto,strlen(texto));

    if(respuesta < 0){
		printf("error : ejecucion, escribir fichero texto=>%s\n", texto);
        exit(-6);
	}

    return;
}

static inline void modificar_buffer(int cantidad)
{
    int respuesta;
    respuesta = ioctl(fd_fichero, IOCTL_MODIFY, cantidad);
	
    if(respuesta < 0){
		printf("error : ejecucion, modificar cantidad=>%d\n", cantidad);
        exit(-10);
	}

	return;
}

