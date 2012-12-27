#include <portaudio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

/* describes all of the characteristics of a playable note */
typedef struct trybloop_note
{
  float frequency;
  float amplitude;
  float duration;
  /* trybloop_waveform waveform; */
} trybloop_note;

/* describes an actively playing note */
typedef struct trybloop_note_active 
{
  trybloop_note note;
  int frame;
} trybloop_note_active;

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
       float frequency = activeNote->note.frequency;
       float amplitude = activeNote->note.amplitude;

       /*  draw a sine wave in buffer using the properties of the active note */
       float *buffer = (float*)outputBuffer; 
       for(int frame = 0; frame < framesPerBuffer; frame++)
       {
           buffer[frame] = amplitude *
                sin( frequency * 2 * M_PI * activeNote->frame / SAMPLE_RATE);
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

   // setup
   Pa_Initialize();

   // set up a note to play
   trybloop_note_active active;
   active.frame = 0;
   active.note.frequency = 440.0;
   active.note.amplitude = 1.0;
   active.note.duration = 10000;

   // play the note
   trybloop_note_play(&active);

   // wait for the specified duration
   Pa_Sleep(active.note.duration);

    // teardown
   Pa_Terminate();
}
