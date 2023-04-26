#ifndef TCC_STKWRAPPER_H
#define TCC_STKWRAPPER_H

#include <stk/BeeThree.h>
#include <stk/Messager.h>
#include <stk/Voicer.h>
#include <stk/SKINImsg.h>
#include <stk/RtAudio.h>
#include <stk/Stk.h>
#include <set>
#include <array>
#include <stk/Messager.h>
#include <stk/Guitar.h>
#include <stk/Mandolin.h>
#include <stk/BeeThree.h>

#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks

class StkWrapper {
    private:
        // stk::Voicer voicer;
        std::array<stk::Instrmnt*, 3> instruments;
        stk::Voicer* voicer;
	    stk::Messager messager;
	    stk::Skini::Message message;
        RtAudio dac{};
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
        void get_sample(int channel = 0);
        stk::StkFrames frames;
        StkWrapper();
        ~StkWrapper();
};

#endif //TCC_STKWRAPPER_H