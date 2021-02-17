

#pragma once

#include "mdaBaseProcessor.h"
#include <dsp.h>

namespace Steinberg {
namespace Vst {
namespace mda {

using algae::dsp::core::filter::vcf_t;
using algae::dsp::core::filter::update_vcf;
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
class TelegraphSynthProcessor : public BaseProcessor
{
public:
	TelegraphSynthProcessor ();
	~TelegraphSynthProcessor ();
	
	int32 getVst2UniqueId () const SMTG_OVERRIDE { return 'MDAj'; }

	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;

	void doProcessing (ProcessData& data) SMTG_OVERRIDE;

	bool hasProgram () const SMTG_OVERRIDE { return true; }
	Steinberg::uint32 getCurrentProgram () const SMTG_OVERRIDE { return currentProgram; }
	Steinberg::uint32 getNumPrograms () const SMTG_OVERRIDE { return kNumPrograms; }
	void setCurrentProgram (Steinberg::uint32 val) SMTG_OVERRIDE;
	void setCurrentProgramNormalized (ParamValue val) SMTG_OVERRIDE;

	enum {
		kNumPrograms = 52
	};
	
	static float programParams[kNumPrograms][24];


	static FUnknown* createInstance (void*) { return (IAudioProcessor*)new TelegraphSynthProcessor; }
	static FUID uid;

protected:
	void processEvents (IEventList* events) SMTG_OVERRIDE;
	void recalculate () SMTG_OVERRIDE;
	void noteOn (int32 note, int32 velocity, int32 noteID);
	void setParameter (ParamID index, ParamValue newValue, int32 sampleOffset) SMTG_OVERRIDE;

	struct VOICE  //voice state
	{
		// float  period;
		// float  p;    //sinc position
		// float  pmax; //loop length
		// float  dp;   //delta
		// float  sin0; //sine osc
		// float  sin1;
		// float  sinx;
		// float  dc;   //dc offset

		// float  detune;
		// float  p2;    //sinc position
		// float  pmax2; //loop length
		// float  dp2;   //delta
		// float  sin02; //sine osc
		// float  sin12;
		// float  sinx2;
		// float  dc2;   //dc offset

		// float  fc;  //filter cutoff root
		// float  ff;  //filter cutoff
		// float  f0;  //filter buffers
		// float  f1;
		// float  f2;

		// float  saw;
		float  vca;  //current level  ///eliminate osc1 level when separate amp & filter envs?
		float  env;  //envelope
		float  att;  //attack
		float  dec;  //decay
		float  envd;
		float  envl;
		float  fenv;
		float  fenvd;
		float  fenvl;

		// float  lev;  //osc levels
		// float  lev2;
		// float  target; //period target
		int32  note; //remember what note triggered this
		bool  noteOn; //remember what note triggered this
		int32 noteID;	// SNA addition
		float snaPitchbend;// SNA addition
		float snaVolume;// SNA addition
		float snaPanLeft;	// SNA addition
		float snaPanRight;	// SNA addition
		vcf_t<double> resonatorState;
		onepole_t<double, double> resDCblock;
		ramp_t<double> exciterEnvState;
		double exciterState;
		double feedbackGain;
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
