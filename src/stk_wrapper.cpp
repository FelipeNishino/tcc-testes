#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stk/BeeThree.h>
#include <vector>
#include "device_manager.hpp"
#include "engine.hpp"
#include "stk_wrapper.hpp"
#include "logger.hpp"

StkWrapper::StkWrapper() {
    stk::Stk::showWarnings(true);
    stk::Stk::setRawwavePath( "./rawwaves/" );
    
    counter = 0;
    no_message = true;
    done = false;
	now_playing = std::set<float>();
    voicer = new stk::Voicer{};
    configure();
    stk::Instrmnt *instrument[3];
    for (int i = 0; i<3; i++ ) instrument[i] = 0;
    try {
        for (int i = 0; i < 3; i++) {
			instrument[i] = new stk::Mandolin(120);
			voicer->addInstrument(instrument[i]);
        }
    }
	catch ( stk::StkError & ) {
        Logger::log(Logger::LOG_ERROR, "<StkWrapper> Erro ao instanciar instrumentos");
	}
}

StkWrapper::~StkWrapper() {
    for (int i=0; i<3; i++ )  delete instruments[i];
    Logger::log(Logger::LOG_ERROR, "<StkWrapper> Deletou instrumentos");
}

bool StkWrapper::has_message() { return !no_message; }
bool StkWrapper::is_done() { return done; }
void StkWrapper::set_done() { done = true; }
void StkWrapper::toggle_stream(bool on) { if (on) dac.startStream(); else dac.stopStream(); }
void StkWrapper::get_sample(int channel) { voicer->tick(frames); }
long StkWrapper::get_message_type() { return message.type; }
long StkWrapper::get_required_counter() { return (long) (message.time * stk::Stk::sampleRate()); }
int StkWrapper::get_counter() { return counter; }
void StkWrapper::set_counter(int n) { counter = n; }

int tick( void *outputBuffer, void *inputBuffer, unsigned int nFrames,
		 double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
	Engine* engine = Engine::GetInstance();
	StkWrapper* wrapper = &engine->wrapper;
    stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;
    int out_channels = nFrames/stk::RT_BUFFER_SIZE;
	int mt, counter = 0;
    int nTicks = (int) nFrames;
	while ( nTicks > 0 && !wrapper->is_done() ) {
		if ( !wrapper->has_message() ) {
			wrapper->message_from_note(engine->get_note());	

			mt = wrapper->get_message_type();
			wrapper->set_counter(( mt > 0 ? wrapper->get_required_counter() : DELTA_CONTROL_TICKS));
		}	
        
        counter = std::min( nTicks, wrapper->get_counter() );
		wrapper->set_counter( wrapper->get_counter() - counter);
    
        wrapper->get_sample();
		for ( int i = 0; i < counter; i++ ) {
            for (int j = 0; j < out_channels; j++) {
                *samples++ = wrapper->frames[i];
            }
			nTicks--;
		}
		if ( nTicks == 0 ) break;

		// Process control messages.
		if ( wrapper->has_message() ) wrapper->process_message();
	}

	return 0;
}

void StkWrapper::process_message() {
	stk::StkFloat value1 = message.floatValues[0];
	stk::StkFloat value2 = message.floatValues[1];
	switch( message.type ) {
		case __SK_Exit_:
			done = true;
			break;

		case __SK_NoteOn_:
			for (auto &note : now_playing)
				voicer->noteOff(note, 64.0);
			now_playing.clear();
			if ( value2 == 0.0 ) // velocity is zero ... really a NoteOff
				voicer->noteOff(value1, 64.0);
			else {
				now_playing.insert(value1);
                voicer->noteOn(value1, value2);
                // instrument->noteOn(220.0 * pow( 2.0, (value1 - 57.0) / 12.0 ), value2 * stk::ONE_OVER_128);
			}
			break;

		case __SK_NoteOff_:
			voicer->noteOff(value1, 64.0);
			break;

		case __SK_ControlChange_:
			voicer->controlChange((int) value1, value2);
			break;

		case __SK_AfterTouch_:
			voicer->controlChange(128, value1);

		case __SK_PitchChange_:
			voicer->setFrequency(value1);
			break;

		case __SK_PitchBend_:
			voicer->pitchBend(value1);
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
    message.floatValues[0] = note;
	message.floatValues[1] = 64;
	message.channel = 1;
	Engine* engine = Engine::GetInstance();
    message.time = ( engine->count_notas == 1 ? 0 : 60.0/double(engine->bpm.load()) );
    // message.time = ( engine->count_notas == 1 ? 0 : engine->get_duration());
    
	// message.time = 0.002;
	// message.time = 0.5;
	no_message = false;
    	// if (message.type == __SK_NoteOn_) {
	// 	message.type = __SK_NoteOff_;
	// 	std::cout << "duracao bpm: " << 60.0/double(engine->bpm.load()) << '\n' << "tempo: " << message.time << '\n'; 
    // 	message.time = 60.0/double(engine->bpm.load()) - message.time;    
	// }
}

void StkWrapper::raise_error() {
    exit(EXIT_FAILURE);
}

void StkWrapper::configure() {
    RtAudio::StreamParameters parameters;
    DeviceManager dm;
	
	parameters.deviceId = dm.get_device_id();
	RtAudio::DeviceInfo device = dac.getDeviceInfo(dm.get_device_id());
	parameters.nChannels = std::max(device.outputChannels, (unsigned int)2);
    Logger::log(Logger::LOG_DEBUG, "Setting nChannels to %d for device %s", parameters.nChannels, device.name.c_str());

	stk::Stk::setSampleRate( device.preferredSampleRate );
	RtAudioFormat format = ( sizeof(stk::StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = stk::RT_BUFFER_SIZE;
    // 1/4
    frames.resize(bufferFrames, parameters.nChannels);
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)stk::Stk::sampleRate(), &bufferFrames, &tick, NULL);
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		raise_error();
	}
}