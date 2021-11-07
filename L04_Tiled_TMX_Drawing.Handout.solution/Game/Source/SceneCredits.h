#ifndef __SCENE_CREDITS_H__	
#define __SCENE_CREDITS_H__	

#include "Module.h"

#include <stdio.h> 

struct SDL_Texture;

class SceneLogo : public Module
{
public:
	
	SceneLogo(bool startEnabled);

	
	~SceneLogo();

	// Called when the module is activated
	// Loads the necessary textures for the map background
	bool Start();

	// Called at the middle of the application loop
	// Updates the scene's background animations
	bool Update() ;

	// Called at the end of the application loop.
	// Performs the render call of all the parts of the scene's background
	bool PostUpdate() ;
	bool CleanUp();

public:
	//background texture
	SDL_Texture* bgTexture = nullptr;

	//Background animation

	SDL_Texture* logo;

	
};

#endif