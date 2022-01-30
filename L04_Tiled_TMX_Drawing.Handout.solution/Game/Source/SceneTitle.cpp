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
	
	//TitleMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	
	//UI
	Creditss = app->tex->Load("Assets/Sprites/UI/Creditos.png");
	Play = app->tex->Load("Assets/Sprites/UI/Start.png");
	Continue = app->tex->Load("Assets/Sprites/UI/Continue.png");
	exit = app->tex->Load("Assets/Sprites/UI/Exit.png");
	settings = app->tex->Load("Assets/Sprites/UI/Setings.png");
	resume = app->tex->Load("Assets/Sprites/UI/Resume.png");
	PRESSQ = app->tex->Load("Assets/Sprites/UI/Pressq.png");

	Logo = app->tex->Load("Assets/textures/logotroll.png");

	intro = app->tex->Load("Assets/textures/scenetitulo.png");
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
		app->render->DrawTexture(PRESSQ, -5, 2400);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_Q)) {
		logo = false;
		
	}
	
	if (Intro == true&& logo == false) {
		app->render->DrawTexture(intro, -100, -120);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		Intro = false;
		app->scene->Menu = false;

	}
	
	if (app->scene->exit == true) {
		salir = true;
	}
	if (app->scene->settings == true) {

	}
	if (logo == false && Intro == true) {
		app->render->DrawTexture(Play, 850, 600);
		app->render->DrawTexture(Creditss, 200, 600);
		app->render->DrawTexture(exit, 200, 450);
		app->render->DrawTexture(settings, 850, 450);
	}
	if (app->scene->credits == true) {
		app->render->DrawTexture(app->scene->Credits, 0, app->scene->credity, NULL);
		app->scene->credity -= dt * 0.220;
	}
	return true;
}


// Called each loop iteration
bool SceneTitle::PostUpdate()
{
	bool ret = true;
	
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN||salir==true)
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