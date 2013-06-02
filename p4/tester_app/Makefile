HEADERS = tester.h 

default: obj

tester.o: tester.c $(HEADERS)
	gcc -c tester.c -o tester.o

obj: tester.o
	gcc tester.o -o tester_app

clean:
	-rm -f tester.o
	-rm -f tester_app
