// Author:			Brent Cowan
// Creation Date:	January 22, 2018
// Modified By:		Tom Tsiliopoulos 
// Date Modified:	January 22, 2018

#include "SoundEngine.h"

// Constructor
SoundEngine::SoundEngine()
{
	init = false;
}

// Deconstructor
SoundEngine::~SoundEngine()
{
	CleanUp();
}

bool SoundEngine::Init()
{
	// Only initialize Sound Engine once
	if (!init) {

		init = true;

		result = FMOD::System_Create(&system);
		FmodErrorCheck(result);
		if (result != FMOD_OK) {
			init = false;
		}

		result = system->getVersion(&version);
		FmodErrorCheck(result);
		if (result != FMOD_OK) {
			init = false;
		}

		result = system->init(100, FMOD_INIT_NORMAL, extradriverdata);
		FmodErrorCheck(result);
		if (result != FMOD_OK) {
			init = false;
		}

		/*
		Set the distance units. (meters/feet etc).
		*/
		result = system->set3DSettings(1.0, 1.0, 1.0f);
		FmodErrorCheck(result);
		if (result != FMOD_OK) {
			init = false;
		}

		sound = new Sound();

	}

	return init;
}

void SoundEngine::Update()
{
	result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);
	FmodErrorCheck(result);

	// Must call this function every frame
	result = system->update();
	FmodErrorCheck(result);
}

void SoundEngine::CleanUp()
{
	// Shutting Down FMOD and releasing resources
	init = false;

	result = system->close();
	FmodErrorCheck(result);
	result = system->release();
	FmodErrorCheck(result);
}

void SoundEngine::setListenerPosition(glm::vec3 newPos)
{
	listener.pos.x = newPos.x;
	listener.pos.y = newPos.y;
	listener.pos.z = newPos.z;
}

void SoundEngine::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD Error: " << FMOD_ErrorString(result) << std::endl;
		//system("pause");
	}
}
