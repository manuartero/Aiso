#include <stdio.h>              /* printf */
#include <stdlib.h>             /* malloc */
#include <unistd.h>             /* exit */
#include <err.h>                /* perror */
#include <getopt.h>             /* getopt */

int main (int argc, char **argv)
{
int optc;
  
    int i= 0;
    printf("argc = %d\n", argc);
    for(i ; i<argc; i++){
        printf("argv[%i] = %s\n", i, argv[i]);
    }    


    // consumimos flags
    while ((optc = getopt (argc, argv, "rwl")) != -1){
        switch (optc){
			case 'r': // READ
                printf("r\n");
				break;

            case 'w': // WRITE
                printf("w\n");
                break;

            case 'l': // LSEEK
                printf("l\n");
                break;

			default : // HELP
                printf("DEFAULT\n");
				break;
		}
    }

	// optind contiene el numero de argumentos
  switch (argc - optind){
    case 0:	// $> test 
      printf("EN 0\n");
      break;

    case 1:	// $> test opc
      printf("EN 1\n");
      break;

    case 2:
    printf("EN 2\n");
break;
    default:	
      warnx ("DEFAULT\n");
      exit(-1);
   }

 exit(0);
}

