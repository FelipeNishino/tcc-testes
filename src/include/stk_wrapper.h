#ifndef TCC_STKWRAPPER_H
#define TCC_STKWRAPPER_H

#include "stk/BeeThree.h"
#include "stk/Messager.h"
#include "stk/Voicer.h"
#include "stk/SKINImsg.h"
#include "stk/RtAudio.h"
#include <stk/Stk.h>

#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks

class StkWrapper {
    private:
        stk::Voicer voicer;
	    stk::Messager messager;
	    stk::Skini::Message message;
        RtAudio dac;
	    int counter;
	    bool no_message;
	    bool done;
        void configure();
        void raise_error();
        // int tick(
        //     void *outputBuffer,
        //     void *inputBuffer,
        //     unsigned int nBufferFrames,
        //     double streamTime,
        //     RtAudioStreamStatus status,
        //     void *dataPointer
        //     );
        void process_message();
        void freq_to_midi_note(float freq);
    public:
        void message_from_note(float note);
        bool has_message();
        bool is_done();
        stk::StkFloat get_sample();
        StkWrapper();
};

#endif //TCC_STKWRAPPER_H