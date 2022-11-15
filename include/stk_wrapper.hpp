#ifndef TCC_STKWRAPPER_H
#define TCC_STKWRAPPER_H

#include "stk/BeeThree.h"
#include "stk/Messager.h"
#include "stk/Voicer.h"
#include "stk/SKINImsg.h"
#include "stk/RtAudio.h"
#include <stk/Stk.h>
#include <set>
#include "stk/BlowHole.h"
#include "stk/VoicForm.h"
#include "stk/Guitar.h"
#include "stk/Saxofony.h"
#include "stk/FM.h"
#include "stk/BandedWG.h"
#include "stk/Flute.h"
#include "stk/Shakers.h"
#include "stk/Mandolin.h"
#include "stk/Mesh2D.h"
#include "stk/StifKarp.h"
#include "stk/Bowed.h"
#include "stk/Brass.h"
#include "stk/Clarinet.h"
#include "stk/Modal.h"
#include "stk/Recorder.h"
#include "stk/BlowBotl.h"
#include "stk/Whistle.h"
#include "stk/Resonate.h"
#include "stk/Simple.h"
#include "stk/FMVoices.h"
#include "stk/Plucked.h"
#include "stk/Rhodey.h"
#include "stk/Wurley.h"
#include "stk/BeeThree.h"
#include "stk/Moog.h"
#include "stk/Sitar.h"
#include "stk/TubeBell.h"
#include "stk/Drummer.h"
#include "stk/PercFlut.h"
#include "stk/HevyMetl.h"
#include "stk/Sampler.h"

#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks

class StkWrapper {
    private:
        // stk::Voicer voicer;
        stk::Voicer voicer;
	    stk::Messager messager;
	    stk::Skini::Message message;
        RtAudio dac;
	    int counter;
	    bool no_message;
	    bool done;
        std::set<float> now_playing;
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
        // float freq_to_midi_note(float freq);
    public:
        void list_devices();
        void process_message();
        void message_from_note(int note);
        long get_message_type();
        bool has_message();
        bool is_done();
        void set_done();
        int get_counter();
        void set_counter(int n);
        void toggle_stream(bool on);
        long get_required_counter();
        stk::StkFloat get_sample();
        StkWrapper();
};

#endif //TCC_STKWRAPPER_H