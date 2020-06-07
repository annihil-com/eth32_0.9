// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

CSounds sounds;


void CSounds::processSounds( void ) 
{
	spree++;

	if( (eth32.cg.time - soundDelay) > 10000 )
	{
		spree = 0;
		soundDelay = eth32.cg.time;
	}

	if(spree < 0 || spree >= SOUND_LEVEL_MAX)
		spree = (SOUND_LEVEL_MAX - 1);

	PlaySound(NULL, NULL, SND_ASYNC);// force stop playing sound

	randNum = (rand() % (SOUND_LEVEL_SIZE_MAX));

	sprintf_s(buf, sizeof(buf), "sound%i%i", this->spree, this->randNum);
	GetPrivateProfileString("Sound Settings", buf, NULL, value, sizeof(value), settingsFile);

	sprintf_s(soundFile, MAX_PATH, "%s\\%s", eth32.path, value);

	if(soundFile)
		PlaySound(soundFile, NULL, SND_ASYNC);

#ifdef ETH32_DEBUG
	Debug.Log("Playing sound: %s", soundFile);
#endif

}

const char *defaultSounds[25] =
{
	"sounds/wowMan.wav",
	"sounds/knee.wav",
	"sounds/excellent.wav",
	"sounds/woop.wav",
	"sounds/yeahBaby.wav",
	"sounds/doublekill.wav",//
	"sounds/omg.wav",
	"sounds/boomhs.wav",
	"sounds/godlike.wav",
	"sounds/domw.wav",
	"sounds/hattrick.wav",//
	"sounds/multikillw.wav",
	"sounds/multikill.wav",
	"sounds/wickedsick.wav",
	"sounds/triple.wav",
	"sounds/rampage.wav",
	"sounds/triple2.wav",//
	"sounds/ultrakill.wav",
	"sounds/unstoppable.wav",
	"sounds/monsterkill.wav",
	"sounds/monsterkillw.wav",
	"sounds/holy.wav",
	"sounds/unreal.wav",//
	"sounds/ludic.wav",
	"sounds/holy.wav"
};

void CSounds::registerSounds( void )
{
	sprintf_s(settingsFile, MAX_PATH, "%s\\%s", eth32.path, ETH32_SETTINGS);


	eth32.cg.media.sounds.hitsound = Syscall.S_RegisterSound("sounds/etprohitsound.wav", qtrue);
	eth32.cg.media.sounds.hitsoundtm = Syscall.S_RegisterSound("sounds/etprohittm.wav", qtrue);
	
	this->spree = -1;

#ifdef ETH32_DEBUG
	Debug.Log("All Game Sounds Registered...");
#endif
}

	
