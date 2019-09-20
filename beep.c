#include <portaudio.h>
#include <math.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

/* A "beep_head" represents an actively playing stream of audio */
struct beep_head;

/* A "beep_waveform" function is a function that can generate a waveform */
typedef float (*beep_waveform)(struct beep_head*);

/* A "beep_note" represents a playable note */
typedef struct beep_note
{
  float frequency;
  float amplitude;
  float duration;
  beep_waveform waveform;
} beep_note;

/* A "beep_head" represents an actively playing stream of audio */
typedef struct beep_head 
{
  beep_note* note;
  int frame;
} beep_head;

/* Generates a sine waveform */
static float beep_waveform_sine(beep_head *head)
{
   beep_note *note = head->note;
   return note->amplitude * sin(2 * M_PI * note->frequency * head->frame / SAMPLE_RATE);
}

/* initialize PortAudio */
static void portaudio_init() {
    Pa_Initialize();
}

/* teardown PortAudio */
static void portaudio_teardown() {
    Pa_Terminate();
}

/* The following callback is called repeatedly by PortAudio to fill a sound device's buffer with audio samples (frames).  We've defined our buffer to consist of 512 frames, and each frame is simply a float32. */
static int beep_head_callback(
	const void *inputBuffer,
	void *outputBuffer, 
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, 
	PaStreamCallbackFlags statusFlags,
	void *data)
{ 
       /* get the active head */
       beep_head* head = (beep_head*)(data);

       /*  draw the waveform into the buffer using the properties of the active head */
       float *buffer = (float*)outputBuffer; 
       for(int frame = 0; frame < framesPerBuffer; frame++)
       {
           buffer[frame] = head->note->waveform(head);
/* printf("freq %f amp %f frame %d buffer %f\n", frequency,amplitude,head->frame,buffer[frame]); */
           head->frame++;
       }
       return paContinue; /* 0 == continue */
}

static void beep_note_play(beep_note* note)
{
    /* start a new tape head at frame 0 */
    beep_head head = {
        .frame = 0,
        .note = note
    };

    /* create a PortAudio stream */
    PaStream * stream;
    Pa_OpenDefaultStream(
    &stream,          /* result stream */
        0, 1,             /* # input channels, # output channels */
        paFloat32,        /* sample format */
        SAMPLE_RATE,      /* sample rate */
        FRAMES_PER_BUFFER,/* frames per buffer */
        beep_head_callback,   /* user-defined callback function */
        &head              /* user-defined callback argument */
    );

    /* start the PortAudio stream */
    Pa_StartStream(stream);

    /* wait for the specified duration */
    Pa_Sleep(note->duration);

    /* stop the PortAudio stream */
    Pa_AbortStream(stream);
}

int main(int argc, char *argv[])
{
    float freq = 0;
    if(argc != 2 || sscanf(argv[1],"%f",&freq)<0)
    {
       printf("beep: error: usage: beep freq\n");
       return 1;
    }

    /* initialize PortAudio */
    portaudio_init();

    /* set up a note to play */
    beep_note note = {
       .frequency = freq,
       .amplitude = 1.0,
       .duration = 1000,
       .waveform = beep_waveform_sine
    };

    /* play the note */
    beep_note_play(&note);

    /* teardown PortAudio */
    portaudio_teardown();
}
