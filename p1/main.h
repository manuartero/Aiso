#include "clipboard.h"

// Estructura
// -----------
// aisoclip/activar
// aisoclip/desactivar
// aisoclip/monitor
// aisoclip/directorio_principal/...

int modulo_p1_init(void);

void modulo_p1_clean(void);

int leer_activar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_desactivar(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int leer_monitor(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);

int escribir_activar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_desactivar(struct file *file, const char *buffer, unsigned long count, void *data);

int escribir_monitor(struct file *file, const char *buffer, unsigned long count, void *data);
