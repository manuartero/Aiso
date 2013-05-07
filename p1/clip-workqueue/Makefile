obj-m += clip.o
clip-objs := ../utiles/utiles.o clipthread.o clipboard.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
