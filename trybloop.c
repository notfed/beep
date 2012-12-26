#include <portaudio.h>
#include <math.h>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512


struct StreamCallbackData
{
  int frame;
  float frequency;
  float amplitude;
} StreamCallbackData;


int streamCallback(
              const void *inputBuffer,
              void *outputBuffer, 
              unsigned long framesPerBuffer,
              const PaStreamCallbackTimeInfo* timeInfo, 
              PaStreamCallbackFlags statusFlags,
              void *inData
)
{ 
              float *buffer = (float*)outputBuffer; 
              struct StreamCallbackData *data = (struct StreamCallbackData*)inData;

              float frequency = data->frequency;
              float amplitude = data->amplitude;
              
              for(int frame = 0; frame < framesPerBuffer; frame++)
              {
                  float totalFrames = (float)(data->frame++);
                  buffer[frame] = amplitude * sin( frequency * 2 * M_PI * totalFrames / SAMPLE_RATE);
              }
              return paContinue; /* 0 == continue */
}


void PlayNote(float frequency, float amplitude, float time)
{
   // 1000ms@440Hz
   struct StreamCallbackData data = {0};
   data.frequency = frequency;
   data.amplitude = amplitude;

   // create stream
   PaStream * stream;
   Pa_OpenDefaultStream(&stream,      /* result stream */
                    0, 1,             /* # input channels, # output channels */
                    paFloat32,        /* sample format */
                    SAMPLE_RATE,      /* sample rate */
                    FRAMES_PER_BUFFER,/* frames per buffer */
                    streamCallback,   /* user-defined callback function */
                    &data             /* user-defined callback argument */
   );

   // start the stream
   Pa_StartStream(stream);
   Pa_Sleep(time);
   Pa_StopStream(stream);
}

int main()
{

   // setup
   Pa_Initialize();

   PlayNote(440.0, 0.5, 100.0); 
   PlayNote(540.0, 1.0, 100.0); 
   PlayNote(640.0, 1.5, 100.0); 
   PlayNote(740.0, 2.0, 100.0); 

    // teardown
   Pa_Terminate();
}
