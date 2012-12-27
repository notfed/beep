#include <portaudio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512


/* represents a function that can create a waveform */
struct trybloop_note_active;
typedef float (*trybloop_waveform)(struct trybloop_note_active*);

/* describes all of the characteristics of a playable note */
typedef struct trybloop_note
{
  float frequency;
  float amplitude;
  float duration;
  trybloop_waveform waveform;
} trybloop_note;

/* describes an actively playing note */
typedef struct trybloop_note_active 
{
  trybloop_note note;
  int frame;
} trybloop_note_active;

/* a sine waveform */
static float trybloop_note_getframe_sine(trybloop_note_active *active)
{
   trybloop_note *note = &active->note;
   return note->amplitude * sin( note->frequency * 2 * M_PI * active->frame / SAMPLE_RATE);
}

/* gets called to fill a sound device's buffer with audio samples (frames) */
/* our buffer consists of 512 frames, and each frame is a float32 */
static int trybloop_note_active_callback(
	const void *inputBuffer,
	void *outputBuffer, 
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, 
	PaStreamCallbackFlags statusFlags,
	void *data)
{ 
       /* get properties of the active note */
       trybloop_note_active* activeNote = (trybloop_note_active*)(data);

       /*  draw the waveform into the buffer using the properties of the active note */
       float *buffer = (float*)outputBuffer; 
       for(int frame = 0; frame < framesPerBuffer; frame++)
       {
           buffer[frame] = activeNote->note.waveform(activeNote);
//printf("freq %f amp %f frame %d buffer %f\n", frequency,amplitude,activeNote->frame,buffer[frame]);
           activeNote->frame++;
       }
       return paContinue; /* 0 == continue */
}



static void trybloop_note_play(trybloop_note_active* activenote)
{
   // create stream
   PaStream * stream;
   Pa_OpenDefaultStream(
	&stream,          /* result stream */
        0, 1,             /* # input channels, # output channels */
        paFloat32,        /* sample format */
        SAMPLE_RATE,      /* sample rate */
        FRAMES_PER_BUFFER,/* frames per buffer */
        trybloop_note_active_callback,   /* user-defined callback function */
        activenote       /* user-defined callback argument */
   );

   // start the stream
   Pa_StartStream(stream);
}

int main(int argc, char *argv[])
{
   int freq = 0;
   if(argc != 2 || sscanf(argv[1],"%d",&freq)<0)
   {
       printf("trybloop: error: usage: trybloop freq\n");
       _exit(1);
   }

   // setup
   Pa_Initialize();

   // set up a note to play
   trybloop_note_active active;
   active.frame = 0;
   active.note.frequency = freq;
   active.note.amplitude = 1.0;
   active.note.duration = 1000;
   active.note.waveform = trybloop_note_getframe_sine;

   // play the note
   trybloop_note_play(&active);

   // wait for the specified duration
   Pa_Sleep(active.note.duration);

    // teardown
   Pa_Terminate();
}
