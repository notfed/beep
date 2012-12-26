#include <portaudio.h>
#include <math.h>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

int streamCallback(
              const void *inputBuffer,
              void *outputBuffer, 
              unsigned long framesPerBuffer,
              const PaStreamCallbackTimeInfo* timeInfo, 
              PaStreamCallbackFlags statusFlags,
              void *data
)
{ 
              float *buffer = (float*)outputBuffer; 
              for(int frame = 0; frame < framesPerBuffer; frame++)
              {
                  buffer[frame] = sin((float)(frame)/10);
              }
              return paContinue; /* 0 == continue */
}

int main()
{
   // setup
   Pa_Initialize();

   // create stream
   PaStream * stream;
   Pa_OpenDefaultStream(&stream,      /* result stream */
                    0, 1,             /* # input channels, # output channels */
                    paFloat32,        /* sample format */
                    SAMPLE_RATE,      /* sample rate */
                    FRAMES_PER_BUFFER,/* frames per buffer */
                    streamCallback,   /* user-defined callback function */
                    0                 /* user-defined callback argument */
   );

   // start the stream
   Pa_StartStream(stream);

  // Pa_Sleep(10000);

   // wait for the stream to finish
   Pa_StopStream(stream);

    // teardown
   Pa_Terminate();
}
