// bethree.cpp STK tutorial program
#include "BeeThree.h"
#include "RtAudio.h"
using namespace stk;
#include <iostream>
using namespace std;
// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct TickData {
	Instrmnt *instrument;
	StkFloat frequency;
	StkFloat scaler;
	long counter;
	bool done;
	// Default constructor.
	TickData()
		: instrument(0), scaler(1.0), counter(0), done( false ) {}
};
// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
				 double streamTime, RtAudioStreamStatus status, void *userData )
{
	TickData *data = (TickData *) userData;
	register StkFloat *samples = (StkFloat *) outputBuffer;
	for ( unsigned int i=0; i<nBufferFrames; i++ ) {
		*samples++ = data->instrument->tick();

		if ( ++data->counter % 48000 == 0 ) {
			switch (data->counter / 48000) {
				case 0:
					data->frequency = 130;
					break;
				case 1:
					data->frequency = 146;
					break;
				case 2:
					data->frequency = 164;
					break;
				case 3:
					data->frequency = 174;
					break;
				case 4:
					data->frequency = 195;
					break;
				case 5:
					data->frequency = 220;
					break;
				case 6:
					data->frequency = 246;
					break;
			}
			// data->scaler += 0.025;
			// cout << data->scaler << endl;
			// data->frequency = data->counter / 48000 * 4
			
			
			
			
			
			
			
			data->instrument->setFrequency( data->frequency * data->scaler );
		}
	}
	if ( data->counter > 48000 * 4 )
		data->done = true;
	return 0;
}
int main()
{
	// Set the global sample rate and rawwave path before creating class instances.
	Stk::setSampleRate( 48000.0 );
	Stk::setRawwavePath( "rawwaves/" );
	TickData data;
	RtAudio dac;
	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	for (int i = 0; i < dac.getDeviceCount(); i++) {
		std::cout << dac.getDeviceInfo(i).name << " - " << i << '\n';
	}
	unsigned int deviceId = 2;
	parameters.deviceId = 2;
	RtAudio::DeviceInfo device = dac.getDeviceInfo(deviceId);
	cout << "Using device " + device.name << endl;
	parameters.nChannels = 1;
	RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&data );
	}
	catch ( RtAudioError& error ) {
		error.printMessage();
		goto cleanup;
	}
	cout << "Using api " << dac.getCurrentApi();
	try {
		// Define and load the BeeThree instrument
		data.instrument = new BeeThree();
	}
	catch ( StkError & ) {
		goto cleanup;
	}
	data.frequency = 130.0;
	data.instrument->noteOn( data.frequency, 0.5 );
	
	try {
		dac.startStream();
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		goto cleanup;
	}
	// Block waiting until callback signals done.
	while ( !data.done )
		Stk::sleep( 100 );
	
	// Shut down the callback and output stream.
	try {
		dac.closeStream();
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
	}
 cleanup:
	delete data.instrument;
	return 0;
}
