#ifndef __SCENETITLE_H__
#define __SCENETITLE_H__

#include "Module.h"

struct SDL_Texture;

class SceneTitle : public Module
{
public:

	SceneTitle(bool startEnabled);

	// Destructor
	~SceneTitle();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();
	

	// Called before quitting
	bool CleanUp();
	SDL_Texture* Logo = nullptr;
	bool logo = true;
	bool playMusic = false;
	uint TitleMusic = 0;
	bool Intro = true;
	bool salir = false;
	SDL_Texture* intro;

	//UI
	SDL_Texture* Play = nullptr;
	SDL_Texture* Creditss = nullptr;
	SDL_Texture* settings = nullptr;
	SDL_Texture* exit = nullptr;
	SDL_Texture* resume = nullptr;
	SDL_Texture* Continue = nullptr;
	SDL_Texture* PRESSQ = nullptr;
private:
	SDL_Texture* img;
};

#endif // __SCENE_H__