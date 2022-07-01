include ../Make.define

PROGS = main

all:${PROGS}

main: main.o
	${CC} ${CFLAGS} -o $@ main.o ${LIBS}


clean:
	rm -f ${PROGS} ${CLEANFILES}