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
	fondo = app->tex->Load("Assets/Sprites/fairy.png");
	app->map->Load("mapadef.tmx");
	app->audio->PlayMusic("Assets/audio/music/Fairy Tail.ogg");
	lose = app->tex->Load("Assets/Sprites/lose.png");
	Win = app->tex->Load("Assets/Sprites/win.png");

	app->player->position.x = 300;
	app->player->position.y = 3098;
	

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
	app->render->DrawTexture(fondo, -900, -200);
	/*active = false;*/
    // L02: DONE 3: Request Load / Save when pressing L/S
	if(app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		app->LoadGameRequest();

	if(app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		app->SaveGameRequest();

	/*if(app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		app->render->camera.y += 80;*/
		

	if(app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		app->render->camera.y -=1;

	if(app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		app->render->camera.x +=1;

	if(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		app->render->camera.x -=1;
	
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
	app->tex->UnLoad(fondo);
	return true;
}
