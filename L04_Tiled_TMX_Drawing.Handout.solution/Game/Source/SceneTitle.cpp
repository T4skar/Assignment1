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
	marco = app->tex->Load("Assets/Sprites/UI/marco.png");
	music = app->tex->Load("Assets/Sprites/UI/musicon.png");
	musicfx = app->tex->Load("Assets/Sprites/UI/volumenon.png");
	vsync = app->tex->Load("Assets/Sprites/UI/vsync.png");
	full = app->tex->Load("Assets/Sprites/UI/fullscreen.png");
	Salir= app->tex->Load("Assets/Sprites/UI/salir.png");
	musicoff = app->tex->Load("Assets/Sprites/UI/musicoff.png");
	musicfxoff = app->tex->Load("Assets/Sprites/UI/volumenoff.png");
	fulloff= app->tex->Load("Assets/Sprites/UI/fullscreenoff.png");
	vsyncoff = app->tex->Load("Assets/Sprites/UI/vsyncoff.png");
	continuar = app->tex->Load("Assets/Sprites/UI/Continue.png");

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
	if (verde == false) {


		if (app->scene->exit == true) {
			salir = true;
		}

		if (logo == false && Intro == true) {
			app->render->DrawTexture(Play, 850, 600);
			app->render->DrawTexture(Creditss, 200, 600);
			app->render->DrawTexture(exit, 200, 450);
			app->render->DrawTexture(settings, 850, 450);
			app->render->DrawTexture(continuar, 850, 350);
		}
		if (app->scene->credits == true) {
			app->render->DrawTexture(app->scene->Credits, 0, app->scene->credity, NULL);
			app->scene->credity -= dt * 0.220;
		}

		if (app->scene->settings == true && app->scene->returnto == false) {
			app->render->DrawTexture(marco, 250, 40);

			app->render->DrawTexture(full, 360, 350);
			app->render->DrawTexture(vsync, 360, 450);
			app->render->DrawTexture(Salir, 850, 500);

		}
		if (app->scene->musicon == true && app->scene->returnto == false) {
			app->render->DrawTexture(musicoff, 360, 150);
			app->scene->musicasi = false;
		}
		if (app->scene->musiconfx == true && app->scene->returnto == false) {
			app->render->DrawTexture(musicfxoff, 360, 250);
		}
		if (app->scene->settings == true && app->scene->musicon == false && app->scene->returnto == false) {
			app->render->DrawTexture(music, 360, 150);

		}
		if (app->scene->settings == true && app->scene->musiconfx == false && app->scene->returnto == false) {

			app->render->DrawTexture(musicfx, 360, 250);
		}
		if (app->scene->vsyncon == true && app->scene->returnto == false) {
			app->render->DrawTexture(vsyncoff, 360, 450);
			#define VSYNC false

		}
		if (app->scene->fullscreen == true && app->scene->returnto == false) {
			app->render->DrawTexture(fulloff, 360, 350);

		}
	}
	else {
		if (app->scene->exit == true) {
			salir = true;
		}

		if (logo == false && Intro == true) {
			app->render->DrawTexture(Play, 850, 600);
			app->render->DrawTexture(Creditss, 200, 600);
			app->render->DrawTexture(exit, 200, 450);
			app->render->DrawTexture(settings, 850, 450);
			app->render->DrawTexture(continuar, 850, 350);
		}
		if (app->scene->credits == true) {
			app->render->DrawTexture(app->scene->Credits, 0, app->scene->credity, NULL);
			app->scene->credity -= dt * 0.220;
		}

		if (app->scene->settings == true && app->scene->returnto == false) {
			app->render->DrawTexture(marco, 250, 40);

			app->render->DrawTexture(full, 360, 350);
			app->render->DrawTexture(vsync, 360, 450);
			app->render->DrawTexture(Salir, 850, 500);

		}
		if (app->scene->musicon == true && app->scene->returnto == false) {
			app->render->DrawTexture(musicoff, 360, 150);
			app->scene->musicasi = false;
		}
		if (app->scene->musiconfx == true && app->scene->returnto == false) {
			app->render->DrawTexture(musicfxoff, 360, 250);
		}
		if (app->scene->settings == true && app->scene->musicon == false && app->scene->returnto == false) {
			app->render->DrawTexture(music, 360, 150);

		}
		if (app->scene->settings == true && app->scene->musiconfx == false && app->scene->returnto == false) {

			app->render->DrawTexture(musicfx, 360, 250);
		}
		if (app->scene->vsyncon == true && app->scene->returnto == false) {
			app->render->DrawTexture(vsyncoff, 360, 450);
#define VSYNC false

		}
		if (app->scene->fullscreen == true && app->scene->returnto == false) {
			app->render->DrawTexture(fulloff, 360, 350);

		}
	}
	if (app->scene->musicasi == true) {
		//musica godmode
		if (app->scene->playMusic == false) {
			if (app->player->godmode == true) {
				app->scene->god = app->audio->PlayMusic("Assets/audio/music/godmode.ogg", 0.3f);
			}
			else {
				if (app->scene->playlev1 == false) {
					app->scene->level1 = app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg", 0.3);
					//playlev1 = true;
				}

			}
			app->scene->playMusic = true;
		}

	}

	

	/*if (app->win->fullScreenWindow == false) {
		app->win->fullScreenWindow = true;
		app->win->SwitchScreen();
	}
	else if (app->win->fullScreenWindow == true) {
		app->win->fullScreenWindow = false;
		app->win->SwitchScreen();
	}*/
	return true;
}


// Called each loop iteration
bool SceneTitle::PostUpdate()
{
	//app->scene->returnto = false;
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