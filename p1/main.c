#include "main.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antonio y Manu");
module_init(modulo_p1_init);
module_exit(modulo_p1_clean);

struct proc_dir_entry * directorio_aisoclip;

int modulo_p1_init(void)
{
    int error = 0;

    directorio_aisoclip = crear_directorio("aisoclip");
    if(directorio_aisoclip == NULL){error = -1;}
    
    error = crear_entrada("activar", directorio_aisoclip, leer_activar, escribir_activar);
    error = crear_entrada("desactivar", directorio_aisoclip, leer_desactivar, escribir_desactivar);
    error = crear_entrada("monitor", directorio_aisoclip, leer_monitor, escribir_monitor);

    return error;
}

void modulo_p1_clean(void)
{
    eliminar_sub_entrada("monitor", directorio_aisoclip);
    eliminar_sub_entrada("desactivar", directorio_aisoclip);
    eliminar_sub_entrada("activar", directorio_aisoclip);
}

int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO
    return 0;
}

int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO
    return 0;
}

int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
    // TODO
    return 0;
}

int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // TODO
    return 0;
}

int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // TODO
    return 0;
}

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data)
{
    // TODO
    return 0;
}
