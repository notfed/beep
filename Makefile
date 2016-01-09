trybloop : trybloop.o
	cc -o trybloop trybloop.o -lportaudio -lm
trybloop.o : trybloop.c
	cc -c trybloop.c
