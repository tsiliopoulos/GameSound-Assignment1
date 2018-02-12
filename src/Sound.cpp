#include "Sound.h"

Sound::Sound()
{

}


Sound::~Sound()
{
}


void Sound::setChannelPosition(glm::vec3 newPos)
{
	FMOD_VECTOR position;
	position.x = newPos.x;
	position.y = newPos.y;
	position.z = newPos.z;

	FMOD_VECTOR velocity;
	velocity = velocityZero;

	result = channel->set3DAttributes(&position, &velocity);
	FmodErrorCheck(result);
}

void Sound::cleanUp()
{
	result = file->release();
	FmodErrorCheck(result);
}

void Sound::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD Error: " << FMOD_ErrorString(result) << std::endl;
		//system("pause");
	}
}
