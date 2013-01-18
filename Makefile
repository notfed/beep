trybloop : trybloop.o
	cc -o trybloop trybloop.o -lportaudio
trybloop.o : trybloop.c
	cc -c trybloop.c
