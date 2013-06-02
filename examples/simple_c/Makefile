default: obj

hello.o: hello.c
	gcc -c hello.c -o hello.o

obj: hello.o
	gcc hello.o -o hello

clean:
	-rm -f hello.o
	-rm -f hello
