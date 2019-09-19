beep : beep.o
	cc -o beep beep.o -lportaudio -lm
beep.o : beep.c
	cc -c beep.c
