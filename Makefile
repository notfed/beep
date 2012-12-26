trybloop : trybloop.o
	clang -o trybloop trybloop.o -lportaudio
trybloop.o : trybloop.c
	clang -c trybloop.c
