obj-m += hello6.o
hello6-objs := hello6_init.o hello6_exit.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
  
clean:
	make -C /lib/modules/$shell uname -r)/build M=$(PWD) clean
