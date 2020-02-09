TARGETS=encode decode

all: ${TARGETS}

encode: encode.c list_tree.o code.h
	gccx -o encode encode.c list_tree.o

decode : decode.c list_tree.o code.h
	gccx -o decode decode.c list_tree.o

list_tree.o: list_tree.c code.h
	gccx -c list_tree.c

clean:
	rm -f *.o ${TARGETS}
