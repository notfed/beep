#beep

A simple note-playing program using PortAudio, written in C.    Public Domain.

#Prerequisites

You must have [portaudio](http://www.portaudio.com/).  On Ubuntu this can be installed by running something like:

    sudo apt-get install portaudio19-dev

Otherwise see [portaudio's website](http://www.portaudio.com/).

#Usage

To download and complile:

    git clone git@github.com:notfed/beep.git beep
    cd beep
    make

To play a 440Hz sine wave, run 

    ./beep 440
