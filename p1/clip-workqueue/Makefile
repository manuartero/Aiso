obj-m += clip2.o
clip2-objs := ../utiles/utiles.o clipboard.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
