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
	bool verde = false;
	SDL_Texture* intro;

	//UI
	SDL_Texture* Play = nullptr;
	SDL_Texture* Creditss = nullptr;
	SDL_Texture* settings = nullptr;
	SDL_Texture* exit = nullptr;
	SDL_Texture* resume = nullptr;
	SDL_Texture* Continue = nullptr;
	SDL_Texture* PRESSQ = nullptr;
	SDL_Texture* Salir = nullptr;
	SDL_Texture* marco = nullptr;
	SDL_Texture* full = nullptr;
	SDL_Texture* music = nullptr;
	SDL_Texture* musicfx = nullptr;
	SDL_Texture* musicoff = nullptr;
	SDL_Texture* musicfxoff = nullptr;
	SDL_Texture* vsync = nullptr;
	SDL_Texture* vsyncoff = nullptr;
	SDL_Texture* fulloff = nullptr;
	SDL_Texture* continuar = nullptr;


	//verde
	SDL_Texture* Playv = nullptr;
	SDL_Texture* Creditssv = nullptr;
	SDL_Texture* settingsv = nullptr;
	SDL_Texture* exitv = nullptr;
	SDL_Texture* resumev = nullptr;
	SDL_Texture* Continuev = nullptr;
	SDL_Texture* PRESSQv = nullptr;
	SDL_Texture* Salirv = nullptr;
	SDL_Texture* marcov = nullptr;
	SDL_Texture* fullv = nullptr;
	SDL_Texture* musicv = nullptr;
	SDL_Texture* musicfxv = nullptr;
	SDL_Texture* musicoffv = nullptr;
	SDL_Texture* musicfxoffv = nullptr;
	SDL_Texture* vsyncv = nullptr;
	SDL_Texture* vsyncoffv = nullptr;
	SDL_Texture* fulloffv = nullptr;
	SDL_Texture* continuarv = nullptr;
	
private:
	SDL_Texture* img;
};

#endif // __SCENE_H__