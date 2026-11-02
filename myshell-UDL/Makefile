default: myshell

myshell: myshell.o argparse.o builtin.o
	gcc -o myshell myshell.o argparse.o builtin.o

myshell.o: argparse.h builtin.h myshell.c
	gcc -c myshell.c

argparse.o: argparse.h argparse.c
	gcc -c argparse.c

builtin.o: builtin.h builtin.c
	gcc -c builtin.c

clean:
	rm -f *.o myshell