

#pragma once

#include "mdaBaseProcessor.h"
#include <algae.h>
#include <telegraph_core.h>

namespace Steinberg {
namespace Vst {
namespace mda {

#define NPARAMS  33      //number of parameters
#define SILENCE  0.001f  //voice choking
#define PI       3.1415926535897932f
#define TWOPI    6.2831853071795864f
#define ANALOG   0.002f  //oscillator drift

using algae::dsp::core::filter::vcf_t;
using algae::dsp::core::filter::process;
using algae::dsp::core::filter::onepole_t;
using algae::dsp::core::filter::update_onepole_hip;
using algae::dsp::core::filter::bp_t;
using algae::dsp::core::filter::bp;
using algae::dsp::core::filter::update_bp;
using algae::dsp::core::filter::update_biquad;
using algae::dsp::core::control::ramp_t;
using algae::dsp::core::control::update_adsr;
using algae::dsp::core::control::update_ad;
using algae::dsp::core::oscillator::noise;
using algae::dsp::core::oscillator::phasor_t;
using algae::dsp::core::oscillator::update_phasor;

class TelegraphSynthProcessor : public BaseProcessor
{
public:
	TelegraphSynthProcessor ();
	~TelegraphSynthProcessor ();
	
	int32 getVst2UniqueId () const SMTG_OVERRIDE { return 'teleg'; }

	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;
	static const int NumberOfParameters = NPARAMS;

	void doProcessing (ProcessData& data) SMTG_OVERRIDE;

	bool hasProgram () const SMTG_OVERRIDE { return true; }
	Steinberg::uint32 getCurrentProgram () const SMTG_OVERRIDE { return currentProgram; }
	Steinberg::uint32 getNumPrograms () const SMTG_OVERRIDE { return kNumPrograms; }
	void setCurrentProgram (Steinberg::uint32 val) SMTG_OVERRIDE;
	void setCurrentProgramNormalized (ParamValue val) SMTG_OVERRIDE;

	enum {
		kNumPrograms = 2
	};
	
	static float programParams[kNumPrograms][NPARAMS];

	static FUnknown* createInstance (void*) { return (IAudioProcessor*) new TelegraphSynthProcessor; }
	static FUID uid;

protected:
	void processEvents (IEventList* events) SMTG_OVERRIDE;
	void recalculate () SMTG_OVERRIDE;
	void noteOn (int32 note, int32 velocity, int32 noteID);
	void setParameter (ParamID index, ParamValue newValue, int32 sampleOffset) SMTG_OVERRIDE;

	struct VOICE  //voice state
	{

		float  vca;  //current level  ///eliminate osc1 level when separate amp & filter envs?
		float  env;  //envelope
		float  att;  //attack
		float  dec;  //decay
		float  envd;
		float  envl;
		float  fenv;
		float  fenvd;
		float  fenvl;

		int32  note; //remember what note triggered this
		bool  noteOn; //remember what note triggered this
		int32 noteID;	// SNA addition
		float snaPitchbend;// SNA addition
		float snaVolume;// SNA addition
		float snaPanLeft;	// SNA addition
		float snaPanRight;	// SNA addition

		telegraph::voice_t<double, double> vox;

	};

	enum {
		EVENTBUFFER = 160,
		EVENTS_DONE = 99999999,
		KMAX = 32,
		SUSTAIN = -1,
		NVOICES = 8
	};

	int32 notes[EVENTBUFFER + 8];  //list of delta|note|velocity for current block

	///global internal variables
	int32 sustain, activevoices;
	VOICE voice[NVOICES];
	telegraph::params_t<double> parameters;

	float semi, cent;
	float tune, detune;
	float filtf, fzip, filtq, filtlfo, filtenv, filtvel, filtwhl;
	float oscmix, noisemix;
	float att, dec, sus, rel, fatt, fdec, fsus, frel;
	float lfo, dlfo, modwhl, press, pbend, ipbend, rezwhl;
	float velsens, volume, voltrim;
	float vibrato, pwmdep, lfoHz, glide, glidedisp;
	int32  K, lastnote, veloff, mode;
	Steinberg::uint32 noise;

	Steinberg::uint32 currentProgram;
};

}}} // namespaces
