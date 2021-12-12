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
#include "Corazones.h"
#include "Checkpoint.h"
#include "SDL/include/SDL_Scancode.h"


#include "Defs.h"
#include "Log.h"

Scene::Scene(bool enabled) : Module()
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
	texture = app->tex->Load("Assets/Sprites/corazones.png");
	texture2 = app->tex->Load("Assets/Sprites/corazones.png");
	texture3 = app->tex->Load("Assets/Sprites/corazones.png");

	//checkpoint = app->tex->Load("Assets/Sprites/Banderas.png");
	lose = app->tex->Load("Assets/Sprites/lose.png");
	Win = app->tex->Load("Assets/Sprites/win.png");
	//winMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	app->player->position.x = 250;
	app->player->position.y = 760;
	//corazon = app->tex->Load("Assets/Sprites/corazones.png");
	//Flag location
	app->checkp->PChpoint.x = 0;
	app->checkp->PChpoint.y = 0;
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
		else  {
			if (playlev1 == false) {
				level1 = app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg", 0.3);
				playlev1 = true;
			}
			
		}
		playMusic = true;
	}
	
	//musica pantalla morir
	/*if (playMusic == false) {
		if (app->player->vidas <= 0) {
			Mdead = app->audio->PlayMusic("Assets/audio/music/lose.ogg", 0.3f);
		}
		else {
			if (playlev1 == false) {
				level1 = app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg", 0.3);
				playlev1 = true;
			}
		}
		playMusic = true;
	}*/


	//if ((app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN))		// Reset lvl 1
	//{
	//	app->audio->PlayFx(nextFx);
	//	CleanUp();
	//	

	//}
		

	
	
	app->render->DrawTexture(fondo, 0, 509);
	//app->render->DrawTexture(checkp, 0, 509);
	
	/*active = false;*/
    // L02: DONE 3: Request Load / Save when pressing F3/F4
	if(app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->LoadGameRequest();

	if(app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		app->SaveGameRequest();
	
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN ) {

		app->player->Win = true;

	}
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN ) {

		app->player->dead = true;
	}

	//Checkpoint teleportation
	app->checkp->PChpoint.x = 0;
	app->checkp->PChpoint.y = 0;
	app->checkp->active = false;
	app->checkp->checkpoint = false;
	app->checkp->BlueFlag = false;
	app->SaveGameRequest();


	//Teletransportación por muerte
	/*if (app->player->vidaDown == true && app->player->vides != 0)
	{
		if (app->checkp->BlueFlag == false)
		{
			app->player->PPlayer.x = 150;
			app->player->PPlayer.y = 875;
			app->player->vidaDown = false;
		}

		else
		{
			app->player->PPlayer.x = 1500;
			app->player->PPlayer.y = 855;
			app->player->vidaDown = false;
		}
	}*/

	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	// Draw map
	app->map->Draw();

	// L03: DONE 7: Set the window title with map/tileset info
	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
				   app->map->mapData.width, app->map->mapData.height,
				   app->map->mapData.tileWidth, app->map->mapData.tileHeight,
				   app->map->mapData.tilesets.count());

	app->win->SetTitle(title.GetString());*/

	return true;
}


// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	
	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	if (app->player->vidas == 2) {

		app->render->DrawTexture(texture2, app->corazon->position.x + 72, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);
	}

	if (app->player->vidas <= 1) {

		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);

	}



	if (app->player->vidas >= 3) {
		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture2, app->corazon->position.x + 72, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture3, app->corazon->position.x + 144, app->corazon->position.y, 0, 0, 0);
	}
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	dWin = false;
	dLose = false;
	playMusic = false;
	playlev1 = false;
	app->tex->UnLoad(fondo);
	return true;
}
