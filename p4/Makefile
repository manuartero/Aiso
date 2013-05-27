obj-m += p4.o
p4-objs := driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
