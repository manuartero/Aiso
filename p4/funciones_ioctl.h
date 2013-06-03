#ifndef FUNCIONES_IOCTL_H
#define FUNCIONES_IOCTL_H

#include <linux/ioctl.h>

/** http://stackoverflow.com/questions/10633311/dynamic-registeration-with-ioctls */

#define NUM_UNICO 0xF5
#define NOMBRE_DEV "aiso_device"
#define RUTA_FICHERO "/dev/aiso_device"

#define IOCTL_RESET   		_IOR(NUM_UNICO, 5, char *)
#define IOCTL_MODIFY  		_IOR(NUM_UNICO, 6, int)
#define IOCTL_WRITTEN 		_IOR(NUM_UNICO, 7, int *)
#define IOCTL_POINTER 		_IOR(NUM_UNICO, 8, int *)


#endif

