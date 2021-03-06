#pragma once
#ifndef __MODULEFADETOBLACK_H__
#define __MODULEFADETOBLACK_H__

#include "Module.h"

class ModuleFadeToBlack : public Module
{
public:
	ModuleFadeToBlack(bool startEnabled);

	virtual ~ModuleFadeToBlack();

	bool Start()override;

	bool Update(float dt) override;

	bool PostUpdate()override;
	//call another module and starts the proces to fade
	bool FadeToBlack(Module* toDisable, Module* toEnable, float frames = 60);
private:

	enum Fade_Step
	{
		NONE,
		TO_BLACK,
		FROM_BLACK
	}currentStep= Fade_Step::NONE;
	//frame count
	Uint32 frameCount = 0;
	Uint32 maxFadeFrames = 0;
	//rectangle of the screen
	SDL_Rect screenRect;

	Module* moduleToEnable = nullptr;
	Module* moduleToDisable = nullptr;
};

#endif //__MODULEFADETOBLACK_H__