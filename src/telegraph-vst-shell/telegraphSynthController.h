
#pragma once

#include "mdaBaseController.h"
#include "telegraphSynthProcessor.h"

namespace Steinberg {
namespace Vst {
namespace mda {

class TelegraphSynthController : public BaseController
{
public:
	TelegraphSynthController ();
	~TelegraphSynthController ();
	
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	tresult PLUGIN_API setParamNormalized (ParamID tag, ParamValue value) SMTG_OVERRIDE;
	tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string) SMTG_OVERRIDE;
	tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized) SMTG_OVERRIDE;


	static FUnknown* createInstance (void*) { return (IEditController*)new TelegraphSynthController; }
	static FUID uid;
};

}}} 
