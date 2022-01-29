#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneTitle.h"
#include "Map.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "Log.h"
#include "SDL/include/SDL_Scancode.h"
#include "Scene.h"
#include "ModuleFadeToBlack.h"

#include "Defs.h"
#include "Log.h"

SceneTitle::SceneTitle(bool enabled) : Module()
{
	name.Create("sceneTitle");
}

// Destructor
SceneTitle::~SceneTitle()
{}

// Called before render is available
bool SceneTitle::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneTitle::Start()
{
	// L03: DONE: Load map
	Logo = app->tex->Load("Assets/textures/logotroll.png");
	
	intro = app->tex->Load("Assets/textures/scenetitulo.png");
	//TitleMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	


	return true;
}

// Called each loop iteration
bool SceneTitle::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneTitle::Update(float dt)
{
	
	if (logo == true) {
		app->render->DrawTexture(Logo, -500, 1700);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_Q)) {
		logo = false;
		
	}
	
	if (Intro == true&& logo == false) {
		app->render->DrawTexture(intro, -500, 1700);
	}
	if (app->input->GetKey(SDL_SCANCODE_R)) {
		Intro = false;
		
	}
	
	



	return true;
}


// Called each loop iteration
bool SceneTitle::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneTitle::CleanUp()
{
	LOG("Freeing scene");
	
	playMusic = false;
	app->tex->UnLoad(Logo);
	return true;
}