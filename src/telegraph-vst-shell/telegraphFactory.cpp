

#include "public.sdk/source/main/pluginfactory.h"
#include "telegraphSynthController.h"
#include "helpers.h"
#include "telegraphVersion.h"

//-----------------------------------------------------------------------------
bool InitModule () { return true; }
bool DeinitModule () { return true; }

//-----------------------------------------------------------------------------
#define kVersionString	FULL_VERSION_STR

using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF (stringCompanyName, 
				   "http://www.steinberg.net", 
				   "mailto:info@steinberg.de")


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// -- Telegraph
DEF_CLASS2 (INLINE_UID_FROM_FUID (mda::TelegraphSynthProcessor::uid),
			PClassInfo::kManyInstances,
			kVstAudioEffectClass,
			"TelegraphSynth",
			Vst::kDistributable,
			Vst::PlugType::kInstrumentSynth,
			kVersionString,
			kVstVersionString,
			mda::TelegraphSynthProcessor::createInstance)

DEF_CLASS2 (INLINE_UID_FROM_FUID (mda::TelegraphSynthController::uid),
			PClassInfo::kManyInstances,
			kVstComponentControllerClass,
			"TelegraphSynth",
			Vst::kDistributable,
			"",
			kVersionString,
			kVstVersionString,
			mda::TelegraphSynthController::createInstance)
//-----------------------------------------------------------------------------


END_FACTORY

