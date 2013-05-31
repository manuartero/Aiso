#include "quien.h"
#define NOMBRE_BUSCADO "utmp"
char *nombre_programa;

//-----------------------------------------//
//---------------- FLAGS ------------------//
static bool salida_breve;
static bool incluye_actividad;
static bool incluye_cabecera;
static bool examina_login;
static bool examina_usuarios;
static bool solo_yo;
//-----------------------------------------//

static char const *formato_tiempo = "%b %e %H:%M";

    
/** Devuelve un string representando el tiempo transcurrido entre CUANDO y AHORA.
 * 	La cadena devuelta es "."  si ha habido actividad ene l termial en el ultimo minuto
 * 	HH:MM, si hace menos de un dia que se ha usado el termoinal 'antig'
 */
static const char * string_actividad (const struct utmp *utmp_ent)
{
  // pasar ut_line de formato corto a formato largo
  char nombre_completo[20];
  char * actual_tty = (char *) (*utmp_ent).ut_line;
  sprintf(nombre_completo, "%s%s", "/dev/", actual_tty);
  
  // consultar fecha de modificado mediante la llamada al sistema stat
  struct stat terminal;
  time_t modificado;
  if (stat(nombre_completo, &terminal) == 0)
  {
      modificado = terminal.st_mtime;	
  }
  
  // time(0) - tiempo de modificado
  time_t ahora = time(NULL);
  time_t dif_int = ahora - modificado;
  
  // cadena de salida; . HH:MM antig
  if ( dif_int < 86400 && dif_int >= 0)
  {
      int segundos = ahora - modificado;
      if (dif_int < 60)
	  return ".";
      else
      {
	  static char actividad[IDLESTR_LEN];
	  sprintf(actividad, "%02d:%02d", segundos / 3600, (segundos % 3600) / 60);
	  return actividad;
      }
  }
  return "antiguo";
}

/** Devuelve un string con la fecha/hora.  */
static char * string_tiempo (const struct utmp *utmp_ent)
{
    static char fmt[64], buf[20];
    struct tm *tm;
    time_t timeGMT = (time_t)(utmp_ent->ut_time);
    if((tm = localtime(&timeGMT)) != NULL)
    {
	strftime(fmt, sizeof(fmt), formato_tiempo, tm);
	snprintf(buf, sizeof(buf), fmt, utmp_ent->ut_tv.tv_usec);
    }
    return buf;
}

/** Construye una cadena "id='campo ut_id' " como comentario*/
static char * id_es_comentario(const struct utmp *utmp_ent)
{
	static char buffer[10];
	sprintf(buffer, "%s%s", "id=", (*utmp_ent).ut_id);
	return buffer;
}

/** Imprime una linea de salida formateada. */
static void print_linea (int longusuario, const char *usuario,
	    int longlinea, const char *linea,
	    const char *str_tiempo, const char *actividad,
	    const char *comentario, const char *str_salida)
{
    if (solo_yo)
    {
	//-m
	printf("%-15s", usuario);
	printf("%-15s", linea);
	printf("%-20s", str_tiempo);
	printf("%-15s", comentario);
    }
    else if (salida_breve)
    {
	//-s
	printf("%-15s",usuario);
	printf("%-15s",linea);
	printf("%-20s",str_tiempo);
	printf("%-15s",comentario);
    }
    else if (examina_usuarios && incluye_actividad)
    {
	//-u
	printf("%-15s",usuario);
	printf("%-15s",linea);
	printf("%-20s",str_tiempo);
	printf("%-15s",actividad);
	printf("%-15s",comentario);
    }
    else if (examina_login && incluye_actividad)
    {
	//-l
	printf("%-15s",usuario);
	printf("%-15s",linea);
	printf("%-20s",str_tiempo);
	printf("%-15s",comentario);
    }
     
    printf("\n");
}

/** Imprime linea para la opcion solo yo*/
static void print_solo_yo (const struct utmp *utmp_ent)
{
    /* Obtener el nombre de la terminal asociada a stdin
    		1) fileno(stdin)
    		2) issatty(int)
    		3) ttynam(int)
    */
    char buf[10];
    char * nombre_tty = "";
    if (isatty(fileno(stdin)))
    {
	    nombre_tty = ttyname(fileno(stdin)); 
    } 
    else
    {
	    perror("stdin no está conectado a un tty"), exit (-1);
    }
    
    char * actual_tty = (char*) utmp_ent->ut_line;
    sprintf(buf, "%s%s", "/dev/", actual_tty);
    if( (strcmp( nombre_tty, buf )) == 0)
    {
	    char comentario[10];
	    sprintf(comentario, "(%s)", utmp_ent->ut_host);
	    print_linea((*utmp_ent).ut_type, (*utmp_ent).ut_user, -1, (*utmp_ent).ut_line, 
		        string_tiempo(utmp_ent), string_actividad(utmp_ent), comentario, "");   
    
    }
}

/** Imprime linea para la opcion login*/
static void print_login (const struct utmp *utmp_ent)
{
    short int type = utmp_ent->ut_type;
    if (type == LOGIN_PROCESS)
    {
	print_linea((*utmp_ent).ut_type, (*utmp_ent).ut_user, -1, 
		    (*utmp_ent).ut_line, string_tiempo(utmp_ent),
		    "", id_es_comentario(utmp_ent), "");
    }
      
}

/** Imprime linea para la opcion cabecera*/
static void print_cabecera (void)
{
    if (solo_yo)
    {
	//-m
	printf("%-15s","NOMBRE");
	printf("%-15s","LINEA");
	printf("%-20s","TIEMPO");
	printf("%-15s","COMENTARIO");
    } 
    else if (salida_breve)
    {
	//-s
	printf("%-15s","NOMBRE");
	printf("%-15s","LINEA");
	printf("%-20s","TIEMPO");
	printf("%-15s","COMENTARIO");
    }
    else if (examina_usuarios && incluye_actividad)
    {
	//-u
	printf("%-15s","NOMBRE");
	printf("%-15s","LINEA");
	printf("%-20s","TIEMPO");
	printf("%-15s","INACTIVIDAD");
	printf("%-15s","COMENTARIO");
    }
    else if (examina_login && incluye_actividad)
    {
	//-l
	printf("%-15s","NOMBRE");
	printf("%-15s","LINEA");
	printf("%-20s","TIEMPO");
	printf("%-15s","COMENTARIO");
    }
    
    printf("\n");  
}

/** Explora el array de registros UTMP_BUF, que debería tener N entradas. */    
static void explorar_entradas (size_t n, const struct utmp *utmp_buf)
{
    // puntero para la entrada utmp actual 
    const struct utmp *aux = (struct utmp *)utmp_buf;
    
    if (incluye_cabecera)
    {
      print_cabecera();
    }
    
    // Recorremos array de tipo Struct utmp
    int i;
    for (i = 0; i < n; i++)
    {  
	    if (solo_yo)
	      // -m
	    {
		    print_solo_yo(aux);
	    }
	    else if (salida_breve) 
	    // -s ; default
	    {	
		    short int type = aux->ut_type;
		    if (type == USER_PROCESS)
		    { 
		        static char comentario[10];
		        sprintf(comentario, "(%s)", (*aux).ut_host);
		        print_linea((*aux).ut_type, (*aux).ut_user, -1, (*aux).ut_line, 
			             string_tiempo(aux),"", comentario, "");
		    }
	    } 
	    else if (examina_usuarios && incluye_actividad)
	    // -u
	    { 
	        short int type = aux->ut_type;
		    if (type == USER_PROCESS)
		    {
		        static char comentario[10];
		        sprintf(comentario, "%s%s%s", "(", aux->ut_host, ")");
		        print_linea((*aux).ut_type, (*aux).ut_user, -1, (*aux).ut_line, 
			    string_tiempo(aux), string_actividad(aux), comentario, "");
		    }
	    } 
	    else if (examina_login && incluye_actividad)
	    // -l
	    {
		    print_login(aux);
	    }
	
	aux++;
	}
}


/** Lee el fichero especificado y almcena su contenido 
   en un array de STRUCT UTMP */
static int leer_utmp(char *fichero, int *n_entradas, struct utmp **utmp_buf) 
{ 
    // 1)
    //Abrir fichero para su consulta
    int fd_fichero;
    int tam;
  
    if ((fd_fichero = open(fichero, O_RDONLY)) == -1)
        perror("open fichero"),exit(-2);
  
    // 2)
    // Consultar tamaño fichero en bytes
    struct stat estadoFichero;
    stat(fichero, &estadoFichero);
  
    // 3) 
    // Reservar en memoria 'tam' bytes
    struct utmp * dirMemoria;
    
    if ((dirMemoria = malloc(estadoFichero.st_size)) == NULL) 
        perror ("error: malloc"),exit(-1);
    
    *utmp_buf = dirMemoria;
  
    // 4)
    // Leo el fichero y calculo el numero de entradas
    if ((tam = read(fd_fichero, dirMemoria, estadoFichero.st_size)) == -1)
    {
        perror("error read"), exit(-3);
    }	
    
    *n_entradas = tam / (sizeof(struct utmp));
    
    return 0;
}

/** Muestra la lista de usuarios conectados al sistema, segunestan registrados
   en el fichero utmp FICHERO.
   Utiliza leer_utmp() para leer el fichero.  */
static void quien (char *argumento)
{
  //me pueden llamar con algo que no existe
  static struct stat infoArgumento;
  int ok = lstat(argumento, &infoArgumento);
  if (ok == -1) perror ("lstat(argumento)"),exit(-1);
  
  //Llamada con un directorio o con un fichero
  // 1) Directorio
  if ( ((infoArgumento.st_mode & S_IFMT)==S_IFDIR) )
  {
      DIR *dirp;
      dirp = opendir(argumento);
     
      if (dirp == NULL)
      { 
	perror("error al abrir el directorio"), exit(-1) ;
      }
      
      //Recorrer el directorio para encontrar un utmp_file
      int encontrado = 0;
      struct dirent *entradasDir; printf("BUSQUEDA \n\n\n");
      
      while ( ((entradasDir = readdir(dirp)) != NULL) && (encontrado == 0) )
      {
	 int comparacion = strcmp("utmp",entradasDir->d_name); 
	 
         if (comparacion == 0)
	 {    
	   encontrado = 1;
	   static char buffer[20];
	   sprintf(buffer, "%s/%s/%s",".", argumento,entradasDir->d_name);
	   printf("veamos argumento: %s\n", argumento);
	   argumento =  buffer;
	   printf("luego: %s\n\n\n", argumento);
	 } 
      }
  
  }
 
  // 2) no era un directorio, preguntar si es un fichero valido
  ok = lstat(argumento, &infoArgumento);
  if (ok == -1) perror ("el fichero dentro dl directorio esta mal"),exit(-1);
     
      
  if ((infoArgumento.st_mode & S_IFMT) == S_IFREG)
  {   
      int n_entradas;
      struct utmp *utmp_buf;

      if (leer_utmp(argumento, &n_entradas, &utmp_buf) != 0)
      {
         err(EXIT_FAILURE, "%s", argumento);
      }  
   
      explorar_entradas (n_entradas, utmp_buf);
      free(utmp_buf); // malloc en leer_utmp 
  }
  
}

/** Muestra la ayuda */
static void uso (int status)
{
    if (status != EXIT_SUCCESS)
        warnx("Pruebe `%s -h' para obtener mas informacion.\n", nombre_programa);
    else
    {
        printf ("Uso: %s [OPCION]... [ FICHERO | ARG1 ARG2 ]\n", nombre_programa);
        fputs ("\
            Muestra informacion sobre los usuarios que actualmente tienen sesion abierta.\n\
            ", stdout);
        fputs ("\
                -H, 	imprime una linea de cabeceras\n\
", stdout);
      fputs ("\
  -l, 	muestra los procesos inicio de sesión (\"login\")\n\
  -m,   equivale a 'soy yo'\n\
", stdout);
      fputs ("\
  -s, 	Muestra solo nombre, terminal y fecha de inicio de sesion (por defecto)\n\
", stdout);
      fputs ("\
  -u, 	Muestra informacion acerca de los usuarios con sesion abierta\n\
  -h,	Muestra este breve recordatorio de uso\n\
", stdout);
      printf ("\
\n\
Si no se especifica FICHERO, usa %s. Es habitual el uso de %s como FICHERO.\n\
Si aparecen ARG1 ARG2, se aplica la opcion -m: `soy yo' es lo usual.\n\
", UTMP_FILE, WTMP_FILE);
    }
  exit (status);
}

/**
 * ./quien2 [fichero o carpeta] [flags -H -h -l -m -s -u] 
 */
int main (int argc, char **argv)
{
  int optc;
  bool pordefecto = true;
  nombre_programa = argv[0];
  
  while ((optc = getopt (argc, argv, "abdlmqrsuHh")) != -1)
  // getopt va consumiendo flags de uno en uno  
  {
      switch (optc)
	{
	  case 'H':
	    incluye_cabecera = true;
	    break;

	  case 'l':
	    examina_login = true;
	    incluye_actividad = true;
	    pordefecto = false;
	    break;

	  case 'm':
	    solo_yo = true;
	    break;

	  case 's':
	    salida_breve = true;
	    break;

	  case 'u':
	    examina_usuarios = true;
	    incluye_actividad = true;
	    pordefecto = false;
	    break;

	  case 'h':
	    uso(EXIT_SUCCESS);

	  default:
	    uso (EXIT_FAILURE);
	}
    }

  if (pordefecto)
    {
      examina_usuarios = true;
      salida_breve = true;
    }

  switch (argc - optind)
    //optind se incrementa +1 cada vez que se ha encontrado un argumentos
    {
    case 2:			/* quien <arg1> <arg2> */
      solo_yo = true;
    case -1:
    case 0:			/* quien */
      quien (UTMP_FILE);
      break;

    case 1:			/* quien <fichero utmp> || <directorio contiene fichero> */
      quien (argv[optind]);
      break;

    default:	
      warnx ("operando de mas: %s", argv[optind + 2]);
      uso (EXIT_FAILURE);
    }

  exit (EXIT_SUCCESS);
}
