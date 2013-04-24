obj-m += p1.o
p1-objs := utiles.o clipthread.o clipboard.o main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
