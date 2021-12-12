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
//#include "Pathfinding.h"

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
	textureC = app->tex->Load("Assets/Sprites/coins2.png");
	// L03: DONE: Load map
	fondo = app->tex->Load("Assets/Sprites/fondo.png");
	app->map->Load("mapadef.tmx");
	texture = app->tex->Load("Assets/Sprites/corazones.png");
	texture2 = app->tex->Load("Assets/Sprites/corazones.png");
	texture3 = app->tex->Load("Assets/Sprites/corazones.png");
	selectFx = app->audio->LoadFx("Assets/fx/salto.wav");
	CheckpointText = app->tex->Load("Assets/textures/Banderas.png");
	lose = app->tex->Load("Assets/Sprites/lose.png");
	Win = app->tex->Load("Assets/Sprites/win.png");
	//winMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	app->player->position.x = 250;
	app->player->position.y = 760;
	//corazon = app->tex->Load("Assets/Sprites/corazones.png");
	//Flag location
	//app->checkp->PChpoint.x = 0;
	//app->checkp->PChpoint.y = 0;
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
		

	
	app->render->DrawTexture(textureC, app->corazon->Cposition.x, app->corazon->Cposition.y, 0, 0, 0);
	app->render->DrawTexture(fondo, 0, 509);
	app->render->DrawTexture(fondo, 2100, 509);
	app->render->DrawTexture(fondo, 4200, 509);
	app->render->DrawTexture(fondo, 6300, 509);
	app->render->DrawTexture(fondo, 8400, 509);
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

	//Checkpoints
	Checkpoints();

	//Teleport
	Teleports();

	StartChTpColliders();

	/*Pathfinding();
	if (app->player->salto==true) {
		app->audio->PlayFx(selectFx);
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

void Scene::Checkpoints()
{
	if (actualScene == 1) {
		if (Point == true && CheckU == false) {
			app->SaveGameRequest();
			Point = false;
			CheckU = true;
		}
		if (Point2 == true && CheckU2 == false) {
			app->SaveGameRequest();
			Point2 = false;
			CheckU2 = true;
		}

		if (Point == false) {
			app->render->DrawTexture(CheckpointText, 70, 255);
		}

		if (Point2 == false) {
			app->render->DrawTexture(CheckpointText, 233, 23);
		}


		if (app->player->CheckActive == true) {
			app->render->DrawTexture(NameCheckText, app->player->position.x - 20, app->player->position.y + 90, NULL);
			app->player->CheckActive = false;
		}
		if (app->player->CheckActive2 == true) {
			app->render->DrawTexture(NameCheckText2, app->player->position.x - 20, app->player->position.y + 90, NULL);
			app->player->CheckActive2 = false;
		}
	}
}

void Scene::Teleports()
{
	if (actualScene == 1) {

		if (ActiveTeleport == true) { //TP 1
			if (tps == 1) {
				app->render->DrawTexture(Teleport2Text, app->player->position.x - 52, app->player->position.y + 80, NULL);
			}
			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN) {
				tps += 1;
			}
			if (tps == 3) {
				tps = 1;
			}
			if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && tps == 1 && Point2 == true) {
				app->player->position.x = 233;
				app->player->position.y = 5;
				ActiveTeleport = false;
			}
			ActiveTeleport = false;
		}

		if (ActiveTeleport2 == true) { //TP 2
			if (tps2 == 1) {
				app->render->DrawTexture(TeleportText, app->player->position.x - 52, app->player->position.y + 80, NULL);
			}
	
			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN) {
				tps2 += 1;
			}
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN) {
				tps2 -= 1;
			}
			if (tps2 == 3 || tps2 == 0) {
				tps2 = 1;
			}
			if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && tps2 == 1 && Point == true) {
				app->player->position.x = 70;
				app->player->position.y = 240;
				ActiveTeleport2 = false;
			
			}
			ActiveTeleport2 = false;
		}
	}

}

void Scene::StartChTpColliders()
{
	//Checkpoints
	CheckP = app->physics->AddCollider({ 70, 260, 20,20 }, Collider::Type::CHECKPOINT, this);
	CheckP2 = app->physics->AddCollider({ 233, 28, 20,20 }, Collider::Type::CHECKPOINT2, this);

	//Teleport
	Teleport = app->physics->AddCollider({ 80,44, 12,12 }, Collider::Type::TELEPORT, this);
}

/*void Scene::Pathfinding()
{
	{int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);
	iPoint mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y);

	app->input->GetMousePosition(mouseX, mouseY);
	iPoint p = app->render->ScreenToWorld(mouseX, mouseY);
	p = app->map->WorldToMap(p.x, p.y);
	p = app->map->MapToWorld(p.x, p.y);

	app->render->DrawTexture(pathTex, p.x, p.y);

	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(pathTex, pos.x, pos.y);
	}

	iPoint originScreen = app->map->MapToWorld(origin1.x, origin1.y);
	app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
	}

}*/
