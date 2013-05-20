#ifndef UTILES_H
#define UTILES_H

#include <linux/kernel.h>	/* kernel */
#include <linux/module.h>	/* modulo */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */

#define NOMBRE_DIRECTORIO_PRINCIPAL "aisoclip"

// Boolean
typedef int boolean;
#define true 1
#define false 0

/* Funciones auxiliares */ 
int mi_atoi(const char* p);
int foo (char c);

/* Funciones para crear-destruir estradas o directorios */
struct proc_dir_entry * crear_directorio(const char * nombre_directorio);

struct proc_dir_entry * crear_sub_directorio(const char * nombre_directorio, struct proc_dir_entry * directorio_padre);

int crear_entrada(const char* nombre_entrada, struct proc_dir_entry *directorio, 
                  int (*leer) (char*, char**, off_t, int, int*, void*), 
                  int (*escribir) (struct file*, const char*, unsigned long, void*) ); 
    
inline void eliminar_entrada(char * entrada);

inline void eliminar_sub_entrada(char * entrada, struct proc_dir_entry *parent);


#endif /* UTILES_H */

