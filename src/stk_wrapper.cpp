#include "include/stk_wrapper.h"
#include "stk/BeeThree.h"
#include "stk/RtAudio.h"
#include "stk/Messager.h"
#include "stk/Voicer.h"
#include "stk/SKINImsg.h"
#include "stk/RtAudio.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

StkWrapper::StkWrapper() {
    counter = 0;
    no_message = true;
    done = false;

    stk::Instrmnt *instrument[3];
    try {
	// Define and load the BeeThree instruments
        for ( int i=0; i<3; i++ ) {
            instrument[i] = new stk::BeeThree();
	        voicer.addInstrument( instrument[i]);
        }
    }
	catch ( stk::StkError & ) {
        std::cout << "Erro ao instanciar instrumentos" << std::endl;   
	}
}

bool StkWrapper::has_message() { return !no_message; }
bool StkWrapper::is_done() { return done; }
stk::StkFloat StkWrapper::get_sample() { return voicer.tick(); }

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		 double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
    // (StkWrapper) dataPointer;
    StkWrapper * pwrapper = static_cast<StkWrapper *>(dataPointer);
    // thing * p2 = static_cast<thing *>(pv); // pointer to the same object
    stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;
	int counter, nTicks = (int) nBufferFrames;

	while ( nTicks > 0 && !pwrapper->is_done() ) {
		// if ( !pwrapper->has_message() ) {
		// 	messager.popMessage( message );
		// 	if ( message.type > 0 ) {
		// 		counter = (long) (message.time * stk::Stk::sampleRate());
		// 		no_message = false;
		// 	}
		// 	else
		// 		counter = DELTA_CONTROL_TICKS;
		// }
	
		counter = std::min( nTicks, counter );
		counter -= counter;
	
		for ( int i=0; i<counter; i++ ) {
			*samples++ = pwrapper->get_sample();
			nTicks--;
		}
		if ( nTicks == 0 ) break;
	
		// Process control messages.
		// if ( pwrapper->has_message() ) pwrapper->process_message();
	}

	return 0;
}

void StkWrapper::configure() {
    RtAudio::StreamParameters parameters;
	for (int i = 0; i < dac.getDeviceCount(); i++) {
		std::cout << dac.getDeviceInfo(i).name << " - " << i << '\n';
	}
	unsigned int deviceId = 3;
	parameters.deviceId = deviceId;
	RtAudio::DeviceInfo device = dac.getDeviceInfo(deviceId);
	std::cout << "Using device " + device.name << std::endl;
	parameters.nChannels = 1;
	RtAudioFormat format = ( sizeof(stk::StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = stk::RT_BUFFER_SIZE;
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)stk::Stk::sampleRate(), &bufferFrames, &tick, NULL );
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		raise_error();
	}
}

void StkWrapper::process_message()
{
	stk::StkFloat value1 = message.floatValues[0];
	stk::StkFloat value2 = message.floatValues[1];

	switch( message.type ) {

	case __SK_Exit_:
	done = true;
	return;

	case __SK_NoteOn_:
	if ( value2 == 0.0 ) // velocity is zero ... really a NoteOff
		voicer.noteOff( value1, 64.0 );
	else { // a NoteOn
		voicer.noteOn( value1, value2 );
	}
	break;

	case __SK_NoteOff_:
	voicer.noteOff( value1, value2 );
	break;

	case __SK_ControlChange_:
	voicer.controlChange( (int) value1, value2 );
	break;

	case __SK_AfterTouch_:
	voicer.controlChange( 128, value1 );

	case __SK_PitchChange_:
	voicer.setFrequency( value1 );
	break;

	case __SK_PitchBend_:
	voicer.pitchBend( value1 );

	} // end of switch

	no_message = false;
	return;
}

void StkWrapper::freq_to_midi_note(float freq) {

}

void StkWrapper::message_from_note(float note) {

}

void StkWrapper::raise_error() {
    exit(EXIT_FAILURE);
}