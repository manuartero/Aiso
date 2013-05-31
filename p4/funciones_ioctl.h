#ifndef FUNCIONES_IOCTL_H
#define FUNCIONES_IOCTL_H

#include <linux/ioctl.h>

/** http://stackoverflow.com/questions/10633311/dynamic-registeration-with-ioctls */

#define NUM_UNICO 0xF5

#define IOCTL_READ  _IOR(NUM_UNICO, 0, char *)
#define IOCTL_WRITE _IOR(NUM_UNICO, 1, char *)
#define IOCTL_LSEEK _IOR(NUM_UNICO, 2, int)
#define IOCTL_RESET _IOR(NUM_UNICO, 3, char *)


#endif

