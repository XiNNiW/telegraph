

#include "telegraphSynthProcessor.h"
#include "telegraphSynthController.h"
#include <cmath>

namespace Steinberg {
namespace Vst {
namespace mda {

#define NPARAMS  24      //number of parameters
#define SILENCE  0.001f  //voice choking
#define PI       3.1415926535897932f
#define TWOPI    6.2831853071795864f
#define ANALOG   0.002f  //oscillator drift

float TelegraphSynthProcessor::programParams[kNumPrograms][NPARAMS] = { 
	{1.0f, 0.37f, 0.25f, 0.3f, 0.32f, 0.5f, 0.9f, 0.6f, 0.12f, 0.0f, 0.5f, 0.9f, 0.89f, 0.9f, 0.73f, 0.0f, 0.5f, 1.0f, 0.71f, 0.81f, 0.65f, 0.0f, 0.5f, 0.5f},
	{0.88f, 0.51f, 0.5f, 0.0f, 0.49f, 0.5f, 0.46f, 0.76f, 0.69f, 0.1f, 0.69f, 1.0f, 0.86f, 0.76f, 0.57f, 0.3f, 0.8f, 0.68f, 0.66f, 0.79f, 0.13f, 0.25f, 0.45f, 0.5f},
	// {0.88f, 0.51f, 0.5f, 0.16f, 0.49f, 0.5f, 0.49f, 0.82f, 0.66f, 0.08f, 0.89f, 0.85f, 0.69f, 0.76f, 0.47f, 0.12f, 0.22f, 0.55f, 0.66f, 0.89f, 0.34f, 0.0f, 1.0f, 0.5f},
	// {1.0f, 0.26f, 0.14f, 0.0f, 0.35f, 0.5f, 0.3f, 0.25f, 0.7f, 0.0f, 0.63f, 0.0f, 0.35f, 0.0f, 0.25f, 0.0f, 0.5f, 1.0f, 0.3f, 0.81f, 0.5f, 0.5f, 0.5f, 0.5f},
	// {0.41f, 0.5f, 0.79f, 0.0f, 0.08f, 0.32f, 0.49f, 0.01f, 0.34f, 0.0f, 0.93f, 0.61f, 0.87f, 1.0f, 0.93f, 0.11f, 0.48f, 0.98f, 0.32f, 0.81f, 0.5f, 0.0f, 0.5f, 0.5f},
	// {0.29f, 0.76f, 0.26f, 0.0f, 0.18f, 0.76f, 0.35f, 0.15f, 0.77f, 0.14f, 0.54f, 0.0f, 0.42f, 0.13f, 0.21f, 0.0f, 0.56f, 0.0f, 0.32f, 0.2f, 0.58f, 0.22f, 0.53f, 0.5f},
	// {1.0f, 0.65f, 0.24f, 0.4f, 0.34f, 0.85f, 0.65f, 0.63f, 0.75f, 0.16f, 0.5f, 0.0f, 0.3f, 0.0f, 0.25f, 0.17f, 0.5f, 1.0f, 0.03f, 0.81f, 0.5f, 0.0f, 0.68f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 1.0f, 0.46f, 0.5f, 0.51f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.3f, 0.0f, 0.25f, 0.37f, 0.5f, 1.0f, 0.38f, 0.81f, 0.62f, 0.0f, 0.5f, 0.5f},
	// {0.84f, 0.51f, 0.15f, 0.45f, 0.41f, 0.42f, 0.54f, 0.01f, 0.58f, 0.21f, 0.67f, 0.0f, 0.09f, 1.0f, 0.25f, 0.2f, 0.85f, 1.0f, 0.3f, 0.83f, 0.09f, 0.4f, 0.49f, 0.5f},
	// {0.71f, 0.75f, 0.53f, 0.18f, 0.24f, 1.0f, 0.56f, 0.52f, 0.69f, 0.19f, 0.7f, 1.0f, 0.14f, 0.65f, 0.95f, 0.07f, 0.91f, 1.0f, 0.15f, 0.84f, 0.33f, 0.0f, 0.49f, 0.5f},
	// {0.0f, 0.25f, 0.43f, 0.0f, 0.71f, 0.48f, 0.23f, 0.77f, 0.8f, 0.32f, 0.63f, 0.4f, 0.18f, 0.66f, 0.14f, 0.0f, 0.38f, 0.65f, 0.16f, 0.48f, 0.5f, 0.0f, 0.67f, 0.5f},
	// {0.62f, 0.26f, 0.51f, 0.79f, 0.35f, 0.54f, 0.64f, 0.39f, 0.51f, 0.65f, 0.0f, 0.07f, 0.52f, 0.24f, 0.84f, 0.13f, 0.3f, 0.76f, 0.21f, 0.58f, 0.3f, 0.0f, 0.36f, 0.5f},
	// {0.81f, 1.0f, 0.21f, 0.78f, 0.15f, 0.35f, 0.39f, 0.17f, 0.69f, 0.4f, 0.62f, 0.0f, 0.47f, 0.19f, 0.37f, 0.0f, 0.5f, 0.2f, 0.33f, 0.38f, 0.53f, 0.0f, 0.12f, 0.5f},
	// {0.0f, 0.51f, 0.52f, 0.96f, 0.44f, 0.5f, 0.41f, 0.46f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.25f, 0.15f, 0.5f, 1.0f, 0.32f, 0.81f, 0.49f, 0.0f, 0.83f, 0.5f},
	// {0.48f, 0.51f, 0.22f, 0.0f, 0.0f, 0.5f, 0.5f, 0.47f, 0.73f, 0.3f, 0.8f, 0.0f, 0.1f, 0.0f, 0.07f, 0.0f, 0.42f, 0.0f, 0.22f, 0.21f, 0.59f, 0.16f, 0.98f, 0.5f},
	// {0.0f, 0.51f, 0.5f, 0.83f, 0.49f, 0.5f, 0.55f, 0.75f, 0.69f, 0.35f, 0.5f, 0.0f, 0.56f, 0.0f, 0.56f, 0.0f, 0.8f, 1.0f, 0.24f, 0.26f, 0.49f, 0.0f, 0.07f, 0.5f},
	// {0.75f, 0.51f, 0.5f, 0.83f, 0.49f, 0.5f, 0.55f, 0.75f, 0.69f, 0.35f, 0.5f, 0.14f, 0.49f, 0.0f, 0.39f, 0.0f, 0.8f, 1.0f, 0.24f, 0.26f, 0.49f, 0.0f, 0.07f, 0.5f},
	// {1.0f, 0.25f, 0.2f, 0.81f, 0.19f, 0.5f, 0.3f, 0.51f, 0.85f, 0.09f, 0.0f, 0.0f, 0.88f, 0.0f, 0.21f, 0.0f, 0.5f, 1.0f, 0.46f, 0.81f, 0.5f, 0.0f, 0.27f, 0.5f},
	// {1.0f, 0.25f, 0.2f, 0.72f, 0.19f, 0.86f, 0.48f, 0.43f, 0.94f, 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.61f, 1.0f, 0.32f, 0.81f, 0.5f, 0.0f, 0.27f, 0.5f},
	// {0.97f, 0.26f, 0.3f, 0.0f, 0.35f, 0.5f, 0.8f, 0.4f, 0.52f, 0.0f, 0.5f, 0.0f, 0.77f, 0.0f, 0.25f, 0.0f, 0.5f, 1.0f, 0.3f, 0.81f, 0.16f, 0.0f, 0.0f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 0.65f, 0.35f, 0.5f, 0.33f, 0.76f, 0.53f, 0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 0.25f, 0.0f, 0.55f, 0.25f, 0.3f, 0.81f, 0.52f, 0.0f, 0.14f, 0.5f},
	// {1.0f, 0.26f, 0.22f, 0.64f, 0.82f, 0.59f, 0.72f, 0.47f, 0.34f, 0.34f, 0.82f, 0.2f, 0.69f, 1.0f, 0.15f, 0.09f, 0.5f, 1.0f, 0.07f, 0.81f, 0.46f, 0.0f, 0.24f, 0.5f},
	// {1.0f, 0.26f, 0.22f, 0.71f, 0.35f, 0.5f, 0.67f, 0.7f, 0.26f, 0.0f, 0.5f, 0.48f, 0.69f, 1.0f, 0.15f, 0.0f, 0.5f, 1.0f, 0.07f, 0.81f, 0.46f, 0.0f, 0.24f, 0.5f},
	// {0.49f, 0.25f, 0.66f, 0.81f, 0.35f, 0.5f, 0.36f, 0.15f, 0.75f, 0.2f, 0.5f, 0.0f, 0.38f, 0.0f, 0.25f, 0.0f, 0.6f, 1.0f, 0.22f, 0.19f, 0.5f, 0.0f, 0.17f, 0.5f},
	// {0.37f, 0.51f, 0.77f, 0.71f, 0.22f, 0.5f, 0.33f, 0.47f, 0.71f, 0.16f, 0.59f, 0.0f, 0.0f, 0.0f, 0.25f, 0.04f, 0.58f, 0.0f, 0.22f, 0.15f, 0.44f, 0.33f, 0.15f, 0.5f},
	// {0.5f, 0.51f, 0.17f, 0.8f, 0.34f, 0.5f, 0.51f, 0.0f, 0.58f, 0.0f, 0.67f, 0.0f, 0.09f, 0.0f, 0.25f, 0.2f, 0.85f, 0.0f, 0.3f, 0.81f, 0.7f, 0.0f, 0.0f, 0.5f},
	// {0.23f, 0.51f, 0.38f, 0.0f, 0.35f, 0.5f, 0.33f, 1.0f, 0.5f, 0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.25f, 0.68f, 0.39f, 0.58f, 0.36f, 0.81f, 0.64f, 0.38f, 0.92f, 0.5f},
	// {0.39f, 0.51f, 0.27f, 0.38f, 0.12f, 0.5f, 0.35f, 0.78f, 0.5f, 0.0f, 0.5f, 0.0f, 0.3f, 0.0f, 0.25f, 0.35f, 0.5f, 0.8f, 0.7f, 0.81f, 0.5f, 0.0f, 0.5f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 0.0f, 0.35f, 0.5f, 0.23f, 0.2f, 0.75f, 0.0f, 0.5f, 0.0f, 0.22f, 0.0f, 0.25f, 0.0f, 0.47f, 0.0f, 0.3f, 0.81f, 0.5f, 0.8f, 0.5f, 0.5f},
	// {1.0f, 0.51f, 0.24f, 0.0f, 0.0f, 0.35f, 0.42f, 0.26f, 0.75f, 0.14f, 0.69f, 0.0f, 0.67f, 0.55f, 0.97f, 0.82f, 0.7f, 1.0f, 0.42f, 0.84f, 0.67f, 0.3f, 0.47f, 0.5f},
	// {0.75f, 0.51f, 0.29f, 0.0f, 0.49f, 0.5f, 0.55f, 0.16f, 0.69f, 0.08f, 0.2f, 0.76f, 0.29f, 0.76f, 1.0f, 0.46f, 0.8f, 1.0f, 0.39f, 0.79f, 0.27f, 0.0f, 0.68f, 0.5f},
	// {0.0f, 0.5f, 0.53f, 0.0f, 0.13f, 0.39f, 0.38f, 0.74f, 0.54f, 0.2f, 0.0f, 0.0f, 0.55f, 0.52f, 0.31f, 0.0f, 0.17f, 0.73f, 0.28f, 0.87f, 0.24f, 0.0f, 0.29f, 0.5f},
	// {0.5f, 0.77f, 0.52f, 0.0f, 0.35f, 0.5f, 0.44f, 0.5f, 0.65f, 0.16f, 0.0f, 0.0f, 0.0f, 0.18f, 0.0f, 0.0f, 0.75f, 0.8f, 0.0f, 0.81f, 0.49f, 0.0f, 0.44f, 0.5f},
	// {0.89f, 0.91f, 0.37f, 0.0f, 0.35f, 0.5f, 0.51f, 0.62f, 0.54f, 0.0f, 0.0f, 0.0f, 0.37f, 0.0f, 1.0f, 0.04f, 0.08f, 0.72f, 0.04f, 0.77f, 0.49f, 0.0f, 0.58f, 0.5f},
	// {1.0f, 0.51f, 0.51f, 0.37f, 0.0f, 0.5f, 0.51f, 0.1f, 0.5f, 0.11f, 0.5f, 0.0f, 0.0f, 0.0f, 0.25f, 0.35f, 0.65f, 0.65f, 0.32f, 0.79f, 0.49f, 0.2f, 0.35f, 0.5f},
	// {0.0f, 0.51f, 0.51f, 0.82f, 0.06f, 0.5f, 0.57f, 0.0f, 0.32f, 0.15f, 0.5f, 0.21f, 0.15f, 0.0f, 0.25f, 0.24f, 0.6f, 0.8f, 0.1f, 0.75f, 0.55f, 0.25f, 0.69f, 0.5f},
	// {0.12f, 0.9f, 0.67f, 0.0f, 0.35f, 0.5f, 0.5f, 0.21f, 0.29f, 0.12f, 0.6f, 0.0f, 0.35f, 0.36f, 0.25f, 0.08f, 0.5f, 1.0f, 0.27f, 0.83f, 0.51f, 0.1f, 0.25f, 0.5f},
	// {0.43f, 0.76f, 0.23f, 0.0f, 0.28f, 0.36f, 0.5f, 0.0f, 0.59f, 0.0f, 0.5f, 0.24f, 0.16f, 0.91f, 0.08f, 0.17f, 0.5f, 0.8f, 0.45f, 0.81f, 0.5f, 0.0f, 0.58f, 0.5f},
	// {0.4f, 0.51f, 0.25f, 0.0f, 0.3f, 0.28f, 0.39f, 0.15f, 0.75f, 0.0f, 0.5f, 0.39f, 0.3f, 0.82f, 0.25f, 0.33f, 0.74f, 0.76f, 0.41f, 0.81f, 0.47f, 0.23f, 0.5f, 0.5f},
	// {0.68f, 0.5f, 0.93f, 0.0f, 0.31f, 0.62f, 0.26f, 0.07f, 0.85f, 0.0f, 0.66f, 0.0f, 0.83f, 0.0f, 0.05f, 0.0f, 0.75f, 0.54f, 0.32f, 0.76f, 0.37f, 0.29f, 0.56f, 0.5f},
	// {1.0f, 0.27f, 0.22f, 0.0f, 0.35f, 0.5f, 0.82f, 0.13f, 0.75f, 0.0f, 0.0f, 0.24f, 0.3f, 0.88f, 0.34f, 0.0f, 0.5f, 1.0f, 0.48f, 0.71f, 0.37f, 0.0f, 0.35f, 0.5f},
	// {0.76f, 0.51f, 0.35f, 0.0f, 0.49f, 0.5f, 0.87f, 0.67f, 1.0f, 0.32f, 0.09f, 0.95f, 0.56f, 0.72f, 1.0f, 0.04f, 0.76f, 0.11f, 0.46f, 0.88f, 0.72f, 0.0f, 0.38f, 0.5f},
	// {0.75f, 0.51f, 0.24f, 0.45f, 0.16f, 0.48f, 0.38f, 0.58f, 0.75f, 0.16f, 0.81f, 0.0f, 0.3f, 0.4f, 0.31f, 0.37f, 0.5f, 1.0f, 0.54f, 0.85f, 0.83f, 0.43f, 0.46f, 0.5f},
	// {0.31f, 0.51f, 0.43f, 0.0f, 0.35f, 0.5f, 0.34f, 0.26f, 0.53f, 0.0f, 0.63f, 0.0f, 0.22f, 0.0f, 0.39f, 0.0f, 0.8f, 0.0f, 0.44f, 0.81f, 0.51f, 0.0f, 0.5f, 0.5f},
	// {0.72f, 0.82f, 1.0f, 0.0f, 0.35f, 0.5f, 0.37f, 0.47f, 0.54f, 0.0f, 0.5f, 0.0f, 0.45f, 0.0f, 0.39f, 0.0f, 0.39f, 0.0f, 0.48f, 0.81f, 0.6f, 0.0f, 0.71f, 0.5f},
	// {0.81f, 0.76f, 0.19f, 0.0f, 0.18f, 0.7f, 0.4f, 0.3f, 0.54f, 0.17f, 0.4f, 0.0f, 0.42f, 0.23f, 0.47f, 0.12f, 0.48f, 0.0f, 0.49f, 0.53f, 0.36f, 0.34f, 0.56f, 0.5f},       
	// {0.57f, 0.49f, 0.31f, 0.0f, 0.35f, 0.5f, 0.46f, 0.0f, 0.68f, 0.0f, 0.5f, 0.46f, 0.3f, 1.0f, 0.23f, 0.3f, 0.5f, 1.0f, 0.31f, 1.0f, 0.38f, 0.0f, 0.5f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 0.0f, 0.35f, 0.5f, 0.08f, 0.36f, 0.69f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.96f, 0.5f, 1.0f, 0.92f, 0.97f, 0.5f, 1.0f, 0.0f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 0.0f, 0.35f, 0.5f, 0.16f, 0.85f, 0.5f, 0.28f, 0.5f, 0.37f, 0.3f, 0.0f, 0.25f, 0.89f, 0.5f, 1.0f, 0.89f, 0.24f, 0.5f, 1.0f, 1.0f, 0.5f},
	// {1.0f, 0.37f, 0.51f, 0.0f, 0.35f, 0.5f, 0.0f, 1.0f, 0.97f, 0.0f, 0.5f, 0.02f, 0.2f, 0.0f, 0.2f, 0.0f, 0.46f, 0.0f, 0.3f, 0.81f, 0.5f, 0.78f, 0.48f, 0.5f},
	// {0.0f, 0.25f, 0.5f, 0.0f, 0.76f, 0.94f, 0.3f, 0.33f, 0.76f, 0.0f, 0.68f, 0.0f, 0.59f, 0.0f, 0.59f, 0.1f, 0.5f, 0.0f, 0.5f, 0.81f, 0.5f, 0.7f, 0.0f, 0.5f},
	// {0.5f, 0.41f, 0.23f, 0.45f, 0.77f, 0.0f, 0.4f, 0.65f, 0.95f, 0.0f, 0.5f, 0.33f, 0.5f, 0.0f, 0.25f, 0.0f, 0.7f, 0.65f, 0.18f, 0.32f, 1.0f, 0.0f, 0.06f, 0.5f},
};

#ifdef SMTG_MDA_VST2_COMPATIBILITY
//-----------------------------------------------------------------------------
FUID TelegraphSynthProcessor::uid (0x55C4F1C0, 0xBE0F45B0, 0xBEC34606, 0x7FDDDE49);
#else
//-----------------------------------------------------------------------------
FUID TelegraphSynthProcessor::uid (0x36DC14F0, 0xC3FD4941, 0xBC7720CA, 0x8376AA01);
#endif

//-----------------------------------------------------------------------------
TelegraphSynthProcessor::TelegraphSynthProcessor ()
: currentProgram (0)
{
	setControllerClass (TelegraphSynthController::uid);
	allocParameters (NPARAMS);
}

//-----------------------------------------------------------------------------
TelegraphSynthProcessor::~TelegraphSynthProcessor ()
{
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API TelegraphSynthProcessor::initialize (FUnknown* context)
{
	tresult res = BaseProcessor::initialize (context);
	if (res == kResultTrue)
	{
		addEventInput (USTRING("MIDI in"), 1);
		addAudioOutput (USTRING("Stereo Out"), SpeakerArr::kStereo);

		const float* newParams = programParams[0];
		if (newParams)
		{
			for (int32 i = 0; i < NPARAMS; i++)
				params[i] = newParams[i];
		}

		//initialise...
		for(int32 v=0; v<NVOICES; v++) 
		{
			memset (&voice[v], 0, sizeof (VOICE));
			// voice[v].dp   = voice[v].dp2   = 1.0f;
			// voice[v].saw  = voice[v].p     = voice[v].p2    = 0.0f;
			voice[v].env  = voice[v].envd  = voice[v].envl  = 0.0f;
			voice[v].fenv = voice[v].fenvd = voice[v].fenvl = 0.0f;
			// voice[v].f0   = voice[v].f1    = voice[v].f2    = 0.0f;
			voice[v].note = 0;
			voice[v].vox = telegraph::initVoice<double>(voice[v].vox, getSampleRate());
		}
		notes[0] = EVENTS_DONE;
		lfo = modwhl = filtwhl = press = fzip = 0.0f; 
		rezwhl = pbend = ipbend = 1.0f;
		volume = 0.0005f;
		K = mode = lastnote = sustain = activevoices = 0;
		noise = 22222;
		
		recalculate ();
	}
	return res;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API TelegraphSynthProcessor::terminate ()
{
	return BaseProcessor::terminate ();
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API TelegraphSynthProcessor::setActive (TBool state)
{
	if (state)
		recalculate ();

	return BaseProcessor::setActive (state);
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::setParameter (ParamID index, ParamValue newValue, int32 sampleOffset)
{
	if (index < NPARAMS)
		BaseProcessor::setParameter (index, newValue, sampleOffset);
	else if (index == BaseController::kPresetParam) // program change
	{
		currentProgram = std::min<int32> (kNumPrograms - 1, (int32)(newValue * kNumPrograms));
		const float* newParams = programParams[currentProgram];
		if (newParams)
		{
			for (int32 i = 0; i < NPARAMS; i++)
				params[i] = newParams[i];
		}
	}
	else if (index == BaseController::kModWheelParam) // mod wheel
	{
		newValue *= 127.;
		modwhl = 0.000005f * (newValue*newValue);
	}
	else if (index == BaseController::kPitchBendParam) // pitch bend
	{
		if (newValue <= 1)
			newValue = (newValue - 0.5) * 0x2000;
		ipbend = (float)exp (0.000014102 * (double)newValue);
		pbend = 1.0f / ipbend;
	}
	else if (index == BaseController::kBreathParam)
	{
		newValue *= 127.;
		filtwhl = 0.02f * newValue;
	}
	else if (index == BaseController::kCtrler3Param)
	{
		newValue *= 127.;
		filtwhl = -0.03f * newValue;
	}
	else if (index == BaseController::kExpressionParam)
	{
		newValue *= 127.;
		rezwhl = 0.0065f * (float)(154 - newValue);
	}
	else if (index == BaseController::kAftertouchParam)
	{
		newValue *= 127.;
		press = 0.00001f * (float)(newValue * newValue);
	}
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::setCurrentProgram (Steinberg::uint32 val)
{
	currentProgram = val;
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::setCurrentProgramNormalized (ParamValue val)
{
	setCurrentProgram (std::min<int32> (kNumPrograms - 1, (int32)(val * kNumPrograms)));
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::doProcessing (ProcessData& data)
{
	int32 sampleFrames = data.numSamples;
	
	float* out1 = data.outputs[0].channelBuffers32[0];
	float* out2 = data.outputs[0].channelBuffers32[1];

	int32 event=0, frame=0, frames, v;
	float oL, oR, e, vib, pwm, pb=pbend, ipb=ipbend, gl=glide;
	float x, y, hpf=0.997f, min=1.0f, w=0.0f, ww=noisemix;
	float ff, fe=filtenv, fq=filtq * rezwhl, fx=1.97f-0.85f*fq, fz=fzip;
	int32 k=K;
	Steinberg::uint32 r;

	vib = (float)sin(lfo);
	ff = filtf + filtwhl + (filtlfo + press) * vib; //have to do again here as way that
	pwm = 1.0f + vib * (modwhl + pwmdep);           //below triggers on k was too cheap!
	vib = 1.0f + vib * (modwhl + vibrato);

	if (activevoices>0 || notes[event]<sampleFrames)
	{    
		while (frame<sampleFrames)
		{
			frames = notes[event++];
			if (frames>sampleFrames) frames = sampleFrames;
			frames -= frame;
			frame += frames;

			while (--frames>=0)
			{
				VOICE *V = voice;
				oL = oR = 0.0f;
				float oneSample = 0;

				if (--k<0)
				{
					// control rate update here
					k = KMAX;
				}

				for(v=0; v<NVOICES; v++)  //for each voice
				{ 
					V->vox = telegraph::process<double, double>(V->vox, parameters, getSampleRate());
					oneSample = V->vox.output;
					
					e = V->env;
					if (e > SILENCE)
					{ //Sinc-Loop Oscillator
						
						
						V->env += V->envd * (V->envl - V->env);
						

						if (k==KMAX) //filter freq update at LFO rate
						{
							// more control rate updates here
							if ((V->env+V->envl)>3.0f) { V->envd=dec; V->envl=sus; } //envelopes
							V->fenv += V->fenvd * (V->fenvl - V->fenv);
							if ((V->fenv+V->fenvl)>3.0f) { V->fenvd=fdec; V->fenvl=fsus; }


						}

						
						oneSample = V->env * oneSample * V->snaVolume;
						
						oL += oneSample * V->snaPanLeft;
						oR += oneSample * V->snaPanRight;
					}
					V++;
				}

				*out1++ = oL;
				*out2++ = oR;
			}

			if (frame<sampleFrames)
			{
				int32 note = notes[event++];
				int32 vel  = notes[event++];
				int32 noteID = notes[event++];
				noteOn (note, vel, noteID);
			}
		}

		activevoices = NVOICES;
		for(v=0; v<NVOICES; v++)
		{
			if (voice[v].env<SILENCE)  //choke voices
			{
				voice[v].env = voice[v].envl = 0.0f;
				// voice[v].f0 = voice[v].f1 = voice[v].f2 = 0.0f;
				voice[v].noteOn = false;
				// voice[v].exciterEnvState = ramp_t<double>();
				voice[v].vox = telegraph::resetVoice<double>(voice[v].vox);
				activevoices--;
			}
		}
	}
	else
	{
		// complete empty block
		memset (out1, 0, sampleFrames * sizeof (float));
		memset (out2, 0, sampleFrames * sizeof (float));
	}
	notes[0] = EVENTS_DONE;  //mark events buffer as done
	fzip = fz;
	K = k;
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::processEvents (IEventList* events)
{
	if (events)
	{
		int32 eventPos = 0;
		int32 count = events->getEventCount ();
		for (int32 i = 0; i < count; i++)
		{
			Event e;
			events->getEvent (i, e);
			switch (e.type)
			{
				case Event::kNoteOnEvent:
				{
					notes[eventPos++] = e.sampleOffset;
					notes[eventPos++] = e.noteOn.pitch;
					notes[eventPos++] = e.noteOn.velocity * 127;
					notes[eventPos++] = e.noteOn.noteId;
					break;
				}
				case Event::kNoteOffEvent:
				{
					notes[eventPos++] = e.sampleOffset;
					notes[eventPos++] = e.noteOff.pitch;
					notes[eventPos++] = 0;
					notes[eventPos++] = e.noteOn.noteId;
					break;
				}
				default:
					continue;
			}
			if (eventPos > EVENTBUFFER) eventPos -= 3; //discard events if buffer full!!
		}
		notes[eventPos] = EVENTS_DONE;
	}
}

//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::noteOn (int32 note, int32 velocity, int32 noteID)
{
	float p, l=100.0f; //louder than any envelope!
	int32  v=0, tmp, held=0;

	bool polyMode = (mode < 3);
	bool _glide = !(mode == 0 || mode == 3);
	bool legato = (mode == 1 || mode == 5);

	if (velocity>0) //note on
	{
		
		if (veloff) velocity = 80;

		if (!polyMode) //monophonic
		{
			if (voice[0].note > 0) //legato pitch change
			{
				for(tmp= (NVOICES-1); tmp>0; tmp--)  //queue any held notes
				{
					voice[tmp].note = voice[tmp - 1].note;
				}
				p = tune * (float)exp (-0.05776226505 * ((double)note + ANALOG * (double)v));
				while (p<3.0f || (p * detune)<3.0f) p += p;
				// voice[v].target = p;
				// if ((_glide)==0) voice[v].period = p;
				// voice[v].fc = (float)exp (filtvel * (float)(velocity - 64)) / p;
				voice[v].env += SILENCE + SILENCE; ///was missed out below if returned?
				voice[v].note = note;
				voice[v].noteOn = true;
				// voice[v].exciterEnvState = ramp_t<double>{};
				voice[v].vox = telegraph::noteOn<double>(voice[v].vox, parameters, note, getSampleRate());

				
				voice[v].noteID = noteID;
				voice[v].snaVolume = 1.f;
				voice[v].snaPanLeft = voice[v].snaPanRight = 1.f;
				voice[v].snaPitchbend = 1.f;
				return;
			}
		}
		else //polyphonic 
		{
			for(tmp=0; tmp<NVOICES; tmp++)  //replace quietest voice not in attack
			{
				if (voice[tmp].note > 0) held++;
				if (voice[tmp].env<l && voice[tmp].envl<2.0f) { l=voice[tmp].env;  v=tmp; }
			}
		}  
		p = tune * (float)exp (-0.05776226505 * ((double)note + ANALOG * (double)v));
		while (p<3.0f || (p * detune)<3.0f) p += p;
		// voice[v].target = p;
		// voice[v].detune = detune;
		voice[v].noteOn = true;
		// voice[v].exciterEnvState = ramp_t<double>{};
		voice[v].vox = telegraph::noteOn<double>(voice[v].vox, parameters, note, getSampleRate());

		tmp = 0;
		if (_glide || legato)
		{
			if ((_glide) || held) tmp = note - lastnote; //glide
		}
		// voice[v].period = p * (float)pow (1.059463094359, (double)tmp - glidedisp);
		// if (voice[v].period<3.0f) voice[v].period = 3.0f; //limit min period

		voice[v].note = lastnote = note;
		
		voice[v].noteID = noteID;

		// voice[v].fc = (float)exp (filtvel * (float)(velocity - 64)) / p; //filter tracking

		// voice[v].lev = voltrim * volume * (0.004f * (float)((velocity + 64) * (velocity + 64)) - 8.0f);
		// voice[v].lev2 = voice[v].lev * oscmix;

		if (params[20]<0.5f) //force 180 deg phase difference for PWM
		{
			// if (voice[v].dp>0.0f)
			// {
			// 	p = voice[v].pmax + voice[v].pmax - voice[v].p;
			// 	voice[v].dp2 = -voice[v].dp;
			// }
			// else
			// {
			// 	p = voice[v].p;
			// 	voice[v].dp2 = voice[v].dp;
			// }
			// voice[v].p2 = voice[v].pmax2 = p + PI * voice[v].period;

			// voice[v].dc2 = 0.0f;
			// voice[v].sin02 = voice[v].sin12 = voice[v].sinx2 = 0.0f;
		}

		if (!polyMode) //monophonic retriggering
		{
			voice[v].env += SILENCE + SILENCE;
		}
		else
		{
			if (params[15] < 0.28f) 
			{
			//  voice[v].f0 = voice[v].f1 = voice[v].f2 = 0.0f; //reset filter
			 voice[v].env = SILENCE + SILENCE;
			 voice[v].fenv = 0.0f;
			}
			else 
				voice[v].env += SILENCE + SILENCE; //anti-glitching trick
		}
		voice[v].envl  = 2.0f;
		voice[v].envd  = att;
		voice[v].fenvl = 2.0f;
		voice[v].fenvd = fatt;
		voice[v].snaVolume = 1.f;
		voice[v].snaPanLeft = voice[v].snaPanRight = 1.f;
		voice[v].snaPitchbend = 1.f;
	}
	else //note off
	{
		
		if ((!polyMode) && (voice[0].note==note)) //monophonic (and current note)
		{
			for(v= (NVOICES-1); v>0; v--) 
			{
				if (voice[v].note>0) held = v; //any other notes queued?
			}
			if (held>0)
			{
				voice[v].note = voice[held].note;
				voice[held].note = 0;
				voice[v].noteOn  = false;
				// voice[v].exciterEnvState = ramp_t<double>{};
				voice[v].vox = telegraph::noteOff(voice[v].vox);

				p = tune * (float)exp (-0.05776226505 * ((double)voice[v].note + ANALOG * (double)v));
				while (p<3.0f || (p * detune)<3.0f) p += p;
				// voice[v].target = p;
				// if ((_glide || legato)==0) voice[v].period = p;
				// voice[v].fc = 1.0f / p;
				
			}
			else
			{
				voice[v].envl  = 0.0f;
				voice[v].envd  = rel;
				voice[v].fenvl = 0.0f;
				voice[v].fenvd = frel;
				voice[v].note  = 0;
				voice[v].noteOn  = false;
				// voice[v].exciterEnvState = ramp_t<double>{};
				voice[v].vox = telegraph::noteOff(voice[v].vox);
			}
		}
		else //polyphonic
		{
			for(v=0; v<NVOICES; v++) if (voice[v].note==note) //any voices playing that note?
			{
				if (sustain==0)
				{
					voice[v].envl  = 0.0f;
					voice[v].envd  = rel;
					voice[v].fenvl = 0.0f;
					voice[v].fenvd = frel;
					voice[v].note  = 0;
					voice[v].noteOn  = false;
					voice[v].vox = telegraph::noteOff(voice[v].vox);
					// voice[v].exciterEnvState = ramp_t<double>{};
					// voice[v].vox = telegraph::resetVoice<double>(voice[v].vox);
				}
				else voice[v].note = SUSTAIN;
			}
		}
	}
}
// I think this method is used to update the synth state with the current value of the dsp params
//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::recalculate ()
{
	double ifs = 1.0 / getSampleRate ();

	mode = std::min<int32>(5, (int32)(params[3] * 6));
	noisemix = params[21] * params[21];
	voltrim = (3.2f - params[0] - 1.5f * noisemix) * (1.5f - 0.5f * params[7]);
	noisemix *= 0.06f;
	oscmix = params[0];

	semi = (float)floor(48.0f * params[1]) - 24.0f;
	cent = 15.876f * params[2] - 7.938f;
	cent = 0.1f * (float)floor(cent * cent * cent);
	detune = (float)pow (1.059463094359f, - semi - 0.01f * cent);
	tune = -23.376f - 2.0f * params[23] - 12.0f * (float)floor(params[22] * 4.9);
	tune = getSampleRate () * (float)pow (1.059463094359f, tune);

	vibrato = pwmdep = 0.2f * (params[20] - 0.5f) * (params[20] - 0.5f);
	if (params[20]<0.5f) vibrato = 0.0f;

	lfoHz = (float)exp (7.0f * params[19] - 4.0f);
	dlfo = lfoHz * (float)(ifs * TWOPI * KMAX); 

	filtf = 8.0f * params[6] - 1.5f;
	filtq  = (1.0f - params[7]) * (1.0f - params[7]); ////// + 0.02f;
	filtlfo = 2.5f * params[9] * params[9];
	filtenv = 12.0f * params[8] - 6.0f;
	filtvel = 0.1f * params[10] - 0.05f;
	if (params[10]<0.05f) { veloff = 1; filtvel = 0; } else veloff = 0;

	att = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[15]));
	dec = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[16]));
	sus = params[17];
	rel = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[18]));
	if (params[18]<0.01f) rel = 0.1f; //extra fast release

	ifs *= KMAX; //lower update rate...

	fatt = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[11]));
	fdec = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[12]));
	fsus = params[13] * params[13];
	frel = 1.0f - (float)exp (-ifs * exp (5.5 - 7.5 * params[14]));

	if (params[4]<0.02f) glide = 1.0f; else
	glide = 1.0f - (float)exp (-ifs * exp (6.0 - 7.0 * params[4]));
	glidedisp = (6.604f * params[5] - 3.302f);
	glidedisp *= glidedisp * glidedisp;
}

}}} // namespaces

