all : DES

DES : main.c
	gcc -o DES main.c crypto.c table.c

def : main.c crypto.c
	gcc -E main.c crypto.c > output.i

clean :
	rm -f DES *.i
