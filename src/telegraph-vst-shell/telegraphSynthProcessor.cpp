

#include "telegraphSynthProcessor.h"
#include "telegraphSynthController.h"
#include <cmath>

namespace Steinberg {
namespace Vst {
namespace mda {

float TelegraphSynthProcessor::programParams[kNumPrograms][NPARAMS] = { 
	{0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5},
	// {1.0f, 0.37f, 0.25f, 0.3f, 0.32f, 0.5f, 0.9f, 0.6f, 0.12f, 0.0f, 0.5f, 0.9f, 0.89f, 0.9f, 0.73f, 0.0f, 0.5f, 1.0f, 0.71f, 0.81f, 0.65f, 0.0f, 0.5f, 0.5f, 0.0f, 0.1f, 0.5, 0.5, 0.5, 0.5,0,0},
	// {0.88f, 0.51f, 0.5f, 0.0f, 0.49f, 0.5f, 0.46f, 0.76f, 0.69f, 0.1f, 0.69f, 1.0f, 0.86f, 0.76f, 0.57f, 0.3f, 0.8f, 0.68f, 0.66f, 0.79f, 0.13f, 0.25f, 0.45f, 0.5f, 0.0f, 0.1f, 0.5, 0.5, 0.5, 0.5,0,1},
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
			voice[v].env  = voice[v].envd  = voice[v].envl  = 0.0f;
			voice[v].fenv = voice[v].fenvd = voice[v].fenvl = 0.0f;
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
	auto SR = getSampleRate();
	auto CR = getSampleRate()/KMAX;
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
				float left_sample, right_sample;

				if (--k<0)
				{
					// control rate update here
					k = KMAX;
				}

				for(v=0; v<NVOICES; v++)  //for each voice
				{ 
					e = V->env;
					// if (e > SILENCE)
					if (V->vox.amp_gate || V->vox.amp_envelope.env.value > SILENCE)
					{ //Sinc-Loop Oscillator
						V->vox = telegraph::process<double, double, 1024>(V->vox, parameters, SR);
						left_sample = V->vox.out_left;
						right_sample = V->vox.out_right;

						// V->env += V->envd * (V->envl - V->env);

						if (k==KMAX) //filter freq update at LFO rate
						{
							// more control rate updates here
							V->vox = telegraph::process_control<double,double>(V->vox, parameters, SR, CR);

							if ((V->env+V->envl)>3.0f) { V->envd=dec; V->envl=sus; } //envelopes
							V->fenv += V->fenvd * (V->fenvl - V->fenv);
							if ((V->fenv+V->fenvl)>3.0f) { V->fenvd=fdec; V->fenvl=fsus; }

						}
						
						// left_sample = left_sample;// * V->snaVolume;
						
						oL += left_sample*0.5; //* V->snaPanLeft;
						oR += right_sample*0.5; //* V->snaPanRight;
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
			if (voice[v].vox.amp_envelope.env.value<SILENCE)  //choke voices
			{
				voice[v].env = voice[v].envl = 0.0f;
				voice[v].noteOn = false;
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
	float l=100.0f; //louder than any envelope!
	auto SR = getSampleRate();
	auto CR = getSampleRate()/KMAX;

	if (velocity>0) //note on
	{

		int32  v=0;
		for(int tmp=0; tmp<NVOICES; tmp++)  //replace quietest voice not in attack
		{
			if (voice[tmp].vox.amp_envelope.env.value<l && voice[tmp].vox.amp_envelope.env.value<2.0f) { l=voice[tmp].vox.amp_envelope.env.value;  v=tmp; }
		}
		voice[v].vox = telegraph::noteOn<double>(voice[v].vox, parameters, note, SR, CR);
	}
	else //note off
	{

		for(int tmp=0; tmp<NVOICES; tmp++) if (voice[tmp].vox.note==note) //any voices playing that note?
		{
			voice[tmp].vox = telegraph::noteOff(voice[tmp].vox);
		}

	}
}
// I think this method is used to update the synth state with the current value of the dsp params
//-----------------------------------------------------------------------------
void TelegraphSynthProcessor::recalculate ()
{
	auto SR = getSampleRate();
	auto CR = getSampleRate()/KMAX;

	parameters.resonator_q = telegraph::denormalize_exp<double>(double(params[0]), 0.0, 10.0,100.0); 
	parameters.resonator_feedback = telegraph::denormalize_exp<double>(double(params[1]), 0.0, 100.0, 1000.0); 
	parameters.amp_attack = telegraph::denormalize_exp<double>(double(params[2]), 0.0, 4.0,100.0)*CR; 
	parameters.amp_decay = telegraph::denormalize_exp<double>(double(params[3]), 0.0, 4.0,100.0)*CR; 
	parameters.amp_sustain = double(params[4]); 
	parameters.amp_release = telegraph::denormalize_exp<double>(double(params[5]), 0.0, 4.0,100.0)*CR;
	parameters.vibrato_depth = telegraph::denormalize_exp<double>(double(params[6]), 0.0, 1.0,100.0);
	parameters.vibrato_speed = telegraph::denormalize<double>(double(params[7]), 0.0, 4.0);
	
	parameters.osc_tune = telegraph::denormalize_set<double>(double(params[8]), {0.25, 0.5, 1, 2, 3, 4});
	parameters.resonater_tune_L = telegraph::denormalize_set<double>(double(params[9]), {0.25, 0.5, 1, 1.5, 2, 3, 4});
	parameters.filter_q = telegraph::denormalize<double>(double(params[10]), 0.0, 1.0);
	parameters.shaper_tune = telegraph::denormalize_set<double>(double(params[11]), {0.25, 0.5, 1, 1.25, 1.5, 2, 3, 4});
	parameters.resonator_cross_feedback = telegraph::denormalize_exp<double>(double(params[12]), 0.0, 1.0, 100.0);
	parameters.exciter_gain = telegraph::denormalize<double>(double(params[13]), 0.0, 1.0);
	parameters.shaper_amount = telegraph::denormalize_exp<double>(double(params[14]), 0.0, 0.9, 1000.0);
	parameters.wave_mode = telegraph::denormalize_set<double,telegraph::Wave>(params[15], {telegraph::SINE,telegraph::SQUARE, telegraph::SAW});
	parameters.filter_cutoff = telegraph::denormalize<double>(params[16], 20, 20000.0);
	
}

}}} // namespaces

