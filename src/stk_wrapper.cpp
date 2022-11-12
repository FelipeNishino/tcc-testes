#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>
#include "device_manager.hpp"
#include "engine.hpp"
#include "midi.hpp"
#include "stk_wrapper.hpp"
#include "utils.hpp"

StkWrapper::StkWrapper() {
    counter = 0;
    no_message = true;
    done = false;
	now_playing = std::set<float>();

	configure();

    stk::Instrmnt *instrument[3];
    try {
	// Define and load the BeeThree instruments
        for ( int i=0; i<3; i++ ) {
            instrument[i] = new stk::BeeThree();
	        voicer.addInstrument(instrument[i]);
        }
    }
	catch ( stk::StkError & ) {
        std::cout << "Erro ao instanciar instrumentos" << std::endl;   
	}
}

bool StkWrapper::has_message() { return !no_message; }
bool StkWrapper::is_done() { return done; }
void StkWrapper::set_done() { done = true; }
void StkWrapper::toggle_stream(bool on) { if (on) dac.startStream(); else dac.stopStream(); }
stk::StkFloat StkWrapper::get_sample() { return voicer.tick(); }
long StkWrapper::get_message_type() { return message.type; }
long StkWrapper::get_required_counter() { return (long) (message.time * stk::Stk::sampleRate()); }
int StkWrapper::get_counter() { return counter; }
void StkWrapper::set_counter(int n) { counter = n; }

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		 double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
	Engine* engine = Engine::GetInstance(std::vector<int>());
	StkWrapper* wrapper = &engine->wrapper;
    stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;
	int mt, counter, nTicks = (int) nBufferFrames;
	
	while ( nTicks > 0 && !wrapper->is_done() ) {
		if ( !wrapper->has_message() ) {
			wrapper->message_from_note(engine->get_note());	

			mt = wrapper->get_message_type();
			wrapper->set_counter(( mt > 0 ? wrapper->get_required_counter() : DELTA_CONTROL_TICKS));
		}	

		counter = std::min( nTicks, wrapper->get_counter() );
		wrapper->set_counter( wrapper->get_counter() - counter);

		for ( int i=0; i<counter; i++ ) {
			*samples++ = wrapper->get_sample();
			nTicks--;
		}
		if ( nTicks == 0 ) break;

		// Process control messages.
		if ( wrapper->has_message() ) wrapper->process_message();
		// if (engine->count_notas > 3)
		// 	engine->wrapper.set_done();
	}

	return 0;
}

void StkWrapper::process_message()
{
	stk::StkFloat value1 = message.floatValues[0];
	stk::StkFloat value2 = message.floatValues[1];

	switch( message.type ) {
		case __SK_Exit_:
			done = true;
			break;

		case __SK_NoteOn_:
			for (auto &note : now_playing)
				voicer.noteOff(note, 64.0);
			now_playing.clear();
			if ( value2 == 0.0 ) // velocity is zero ... really a NoteOff
				voicer.noteOff(value1, 64.0);
			else { // a NoteOn

				now_playing.insert(value1);
				// now_playing.insert(value1 + 2);
				// now_playing.insert(value1 + 4);
				voicer.noteOn(value1, value2);
				// voicer.noteOn(value1 + 2, value2);
				// voicer.noteOn(value1 + 4, value2);
			}
			break;

		case __SK_NoteOff_:
			voicer.noteOff(value1, value2);
			break;

		case __SK_ControlChange_:
			voicer.controlChange((int) value1, value2);
			break;

		case __SK_AfterTouch_:
			voicer.controlChange(128, value1);

		case __SK_PitchChange_:
			voicer.setFrequency(value1);
			break;

		case __SK_PitchBend_:
			voicer.pitchBend(value1);
	} // end of switch

	no_message = true;
	return;
}

void StkWrapper::list_devices() {
	for (unsigned int i = 0; i < dac.getDeviceCount(); i++)
		std::cout << dac.getDeviceInfo(i).name << " - " << i << '\n';
}

void StkWrapper::message_from_note(int note) {
	message.type = __SK_NoteOn_;
    // std::cout << "Nota gerada: " << note << std::endl;
	message.floatValues[0] = Midi::NOTE_TO_MIDI_KEY[note];
	// std::cout << "Nota na mensagem: " << message.floatValues[0] << std::endl;
	message.floatValues[1] = 64;
	message.channel = 1;
	Engine* engine = Engine::GetInstance(std::vector<int>());
    message.time = ( engine->count_notas == 1 ? 0 : 60.0/double(engine->bpm.load()) );
    
	// std::cout << engine->count_notas << std::endl;
	// message.time = 0.002;
	// message.time = 0.5;
	no_message = false;
}

void StkWrapper::raise_error() {
    exit(EXIT_FAILURE);
}

void StkWrapper::configure() {
    RtAudio::StreamParameters parameters;
	// list_devices();
    DeviceManager dm;
	
	parameters.deviceId = dm.get_device_id();
	RtAudio::DeviceInfo device = dac.getDeviceInfo(dm.get_device_id());
	parameters.nChannels = 1;
	stk::Stk::setSampleRate( device.preferredSampleRate );
	RtAudioFormat format = ( sizeof(stk::StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = stk::RT_BUFFER_SIZE;
    // 1/4
    
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)stk::Stk::sampleRate(), &bufferFrames, &tick, NULL );
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		raise_error();
	}
}