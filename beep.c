#include <portaudio.h>
#include <math.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

/* A "beep_note_active" represents an actively playing note */
struct beep_note_active;

/* A "beep_waveform" function is a function that can generate a waveform */
typedef float (*beep_waveform)(struct beep_note_active*);

/* A "beep_note" represents a playable note */
typedef struct beep_note
{
  float frequency;
  float amplitude;
  float duration;
  beep_waveform waveform;
} beep_note;

/* A "beep_note_active" represents an actively playing note */
typedef struct beep_note_active 
{
  beep_note note;
  int frame;
} beep_note_active;

/* Generates a sine waveform */
static float beep_waveform_sine(beep_note_active *active)
{
   beep_note *note = &active->note;
   return note->amplitude * sin( note->frequency * 2 * M_PI * active->frame / SAMPLE_RATE);
}

/* The following callback is called repeatedly by PortAudio to fill a sound device's buffer with audio samples (frames).  We've defined our buffer to consist of 512 frames, and each frame is simply a float32. */
static int beep_note_active_callback(
	const void *inputBuffer,
	void *outputBuffer, 
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, 
	PaStreamCallbackFlags statusFlags,
	void *data)
{ 
       /* get properties of the active note */
       beep_note_active* active = (beep_note_active*)(data);

       /*  draw the waveform into the buffer using the properties of the active note */
       float *buffer = (float*)outputBuffer; 
       for(int frame = 0; frame < framesPerBuffer; frame++)
       {
           buffer[frame] = active->note.waveform(active);
/* printf("freq %f amp %f frame %d buffer %f\n", frequency,amplitude,active->frame,buffer[frame]); */
           active->frame++;
       }
       return paContinue; /* 0 == continue */
}



static void beep_note_play(beep_note_active* activenote)
{
   /* create stream */
   PaStream * stream;
   Pa_OpenDefaultStream(
	&stream,          /* result stream */
        0, 1,             /* # input channels, # output channels */
        paFloat32,        /* sample format */
        SAMPLE_RATE,      /* sample rate */
        FRAMES_PER_BUFFER,/* frames per buffer */
        beep_note_active_callback,   /* user-defined callback function */
        activenote       /* user-defined callback argument */
   );

   /* start the stream */
   Pa_StartStream(stream);
}

int main(int argc, char *argv[])
{
   float freq = 0;
   if(argc != 2 || sscanf(argv[1],"%f",&freq)<0)
   {
       printf("beep: error: usage: beep freq\n");
       return 1;
   }

   /* setup */
   Pa_Initialize();

   /* set up a note to play */
   beep_note_active active;
   active.frame = 0;
   active.note.frequency = freq;
   active.note.amplitude = 1.0;
   active.note.duration = 1000;
   active.note.waveform = beep_waveform_sine;

   /* play the note */
   beep_note_play(&active);

   /* wait for the specified duration */
   Pa_Sleep(active.note.duration);

    /* teardown */
   Pa_Terminate();
}
