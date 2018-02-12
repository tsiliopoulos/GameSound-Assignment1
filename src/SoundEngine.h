#pragma once
// Author:			Brent Cowan
// Creation Date:	January 22, 2018
// Modified By:		Tom Tsiliopoulos 
// Date Modified:	January 22, 2018

//#pragma comment (lib, "../lib/fmod_vc.lib")
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include <Windows.h>
#include <math.h>
#include <iostream> // cin & cout
#include <conio.h>
#include <glm/vec3.hpp>

#include "Sound.h"

using namespace std;



struct Listener {
	FMOD_VECTOR     pos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR		vel = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR		forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR		up = { 0.0f, 1.0f, 0.0f };
};

class SoundEngine {
private:
	bool			init;
	FMOD_RESULT     result;

public:
	SoundEngine();
	~SoundEngine();

	bool Init();

	void Update();
	void CleanUp();

	FMOD::System    *system;
	unsigned int    version;
	void            *extradriverdata = 0;
	Listener		listener;

	Sound			*sound;

	void setListenerPosition(glm::vec3 newPos);

	void FmodErrorCheck(FMOD_RESULT result);


};