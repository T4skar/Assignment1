#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"

#include "SDL/include/SDL_Scancode.h"


#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// L03: DONE: Load map
	fondo = app->tex->Load("Assets/Sprites/fondo.png");
	app->map->Load("mapadef.tmx");
	checkpoint = app->tex->Load("Assets/Sprites/Natsu3.png");
	lose = app->tex->Load("Assets/Sprites/lose.png");
	Win = app->tex->Load("Assets/Sprites/win.png");
	//winMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	app->player->position.x = 250;
	app->player->position.y = 760;
	

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//musica godmode
	if (playMusic == false) {
		if (app->player->godmode == true) {
			god = app->audio->PlayMusic("Assets/audio/music/godmode.ogg", 0.3f);
		}
		else {
			level1 = app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg", 0.3);
		}
		playMusic = true;
	}

	//musica pantalla morir
	if (playMusic == false&& app->player->godmode == false) {
		if (app->player->dead == true) {
			Mdead = app->audio->PlayMusic("Assets/audio/music/lose.ogg", 0.3f);
		}
		else {
			level1 = app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg", 0.3);
		}
		playMusic = true;
	}


	if ((app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN))		// Reset lvl 1
	{
		app->audio->PlayFx(nextFx);
		CleanUp();
		

	}
		

	
	
	app->render->DrawTexture(fondo, 0, 509);
	app->render->DrawTexture(checkpoint, 0, 509);
	
	/*active = false;*/
    // L02: DONE 3: Request Load / Save when pressing L/S
	if(app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		app->LoadGameRequest();

	if(app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		app->SaveGameRequest();


	
	
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && app->scene->dLose == false) {

		dWin = true;
		if (dWin == true) {
			app->render->Blit(Win, app->player->position.x, app->player->position.y, 0, 0);
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && app->scene->dWin == false) {

		dLose = true;
		if (dLose == true) {
			app->render->Blit(lose, app->player->position.x, app->player->position.y, 0, 0);
		}
	}

	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	// Draw map
	app->map->Draw();

	// L03: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
				   app->map->mapData.width, app->map->mapData.height,
				   app->map->mapData.tileWidth, app->map->mapData.tileHeight,
				   app->map->mapData.tilesets.count());

	app->win->SetTitle(title.GetString());

	return true;
}


// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	
	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	dWin = false;
	dLose = false;
	playMusic = false;
	app->tex->UnLoad(fondo);
	return true;
}
