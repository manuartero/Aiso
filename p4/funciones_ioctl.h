#ifndef FUNCIONES_IOCTL_H
#define FUNCIONES_IOCTL_H

#include <linux/ioctl.h>

#define IOCTL_READ _IOR (int, 0, char *)
#define IOCTL_WRITE _IOR(int, 1, char *)
#define IOCTL_LSEEK _IOR(int, 2, char*)
#define IOCTL_RESET _IOR(int, 3, char*)


#endif
