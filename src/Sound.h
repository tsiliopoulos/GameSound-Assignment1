#pragma once
// Author:			Brent Cowan
// Creation Date:	January 22, 2018
// Modified By:		Tom Tsiliopoulos 
// Student ID:		100616336
// Date Modified:	February 11, 2018

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include <Windows.h>
#include <math.h>
#include <iostream> // cin & cout
#include <conio.h>
#include <glm/vec3.hpp>

class Sound
{
public:
	Sound();
	~Sound();

	const FMOD_VECTOR positionZero = { 0.0f, 0.0f, 0.0f };
	const FMOD_VECTOR velocityZero = { 0.0f, 0.0f, 0.0f };

	FMOD_RESULT     result;

	FMOD::Sound     *file;

	FMOD::Channel   *channel;


	void setChannelPosition(glm::vec3 newPos);
	void cleanUp();

	void FmodErrorCheck(FMOD_RESULT result);
};

