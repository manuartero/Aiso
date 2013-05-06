#include "directorio.h"

struct proc_dir_entry * directorio_aisoclip;

int crear_directorio_aiso(const char* nombre_directorio){

	int error=0;
	
	directorio_aisoclip = crear_sub_directorio(nombre_directorio, NULL);
    if(directorio_aisoclip == NULL){error = -1;}
    return error;
}
