#pragma once

class CSounds
{
public:
	void	processSounds(  );
	void	registerSounds( void );

private:
	int		randNum;
	int		soundDelay;
	int		spree;
	char	settingsFile[MAX_PATH];

	char	buf[64];
	char	value[1024];
	char	soundFile[MAX_PATH];

};
extern CSounds sounds;

extern const char *defaultSounds[25];