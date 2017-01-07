main:main.o input.o command.o
       gcc -g -o ./main main.o input.o command.o
main.o:main.c
	gcc -g -c main.c
input.o:input.c
	gcc -g -c input.c
command.o:command.c
	gcc -g -c command.c
.PHONY:clean
clean:
	rm *.o

