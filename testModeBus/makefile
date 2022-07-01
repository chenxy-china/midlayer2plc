all:run

run:main.c
	gcc main.c -o run `pkg-config --libs --cflags libmodbus`

clean:
	rm -rf run