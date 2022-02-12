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
#include "ModuleCoin.h"
#include "Checkpoint.h"
#include "GuiManager.h"
#include "SDL/include/SDL_Scancode.h"
#include"SceneTitle.h"
#include"Timer.h"
#include"ModuleFonts.h"

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
	Moneda = app->tex->Load("Assets/Sprites/coin2.png");
	Credits = app->tex->Load("Assets/Sprites/credis.png");


	
	SelectedFx = app->audio->LoadFx("Assets/audio/fx/ui.wav");
	//SelectedFx2 = app->audio->LoadFx("Assets/audio/fx/ui2.wav");


	//font = app->fonts->Load("Assets/Sprites/fonts/font.png");
	//winMusic = app->audio->LoadFx("assets/sound/music/win_sound_loop.ogg");
	app->player->position.x = 250;
	app->player->position.y = 600;
	//corazon = app->tex->Load("Assets/Sprites/corazones.png");
	//Flag location
	//app->checkp->PChpoint.x = 0;
	//app->checkp->PChpoint.y = 0;
	//timer.Start();
	btn1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Test1", { (app->win->GetWidth() / 2) + 280 , app->win->GetWidth() / 2-40, 160, 80 }, this);
	btn2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Test2", { (app->win->GetWidth() / 2) - 400, app->win->GetWidth() / 2-40, 160, 80 }, this);
	btn3 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Test3", { (app->win->GetWidth() / 2) + 280 , app->win->GetWidth() / 2 - 200, 160, 80 }, this);
	btn4 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Test4", { (app->win->GetWidth() / 2) - 400, app->win->GetWidth() / 2 - 200, 160, 80 }, this);
	btn5 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Test5", { (app->win->GetWidth() / 2) - 260 , app->win->GetWidth() / 2-480 , 80, 80 }, this);
	btn6 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Test6", { (app->win->GetWidth() / 2) - 260, app->win->GetWidth() / 2 - 380, 80, 80 }, this);
	btn7 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Test7", { (app->win->GetWidth() / 2) + 40 , app->win->GetWidth() / 2 - 290, 80, 80 }, this);
	btn8 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "Test8", { (app->win->GetWidth() / 2) -100, app->win->GetWidth() / 2 - 200, 80, 80 }, this);
	btn9 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Test9", { (app->win->GetWidth() / 2) +200 , app->win->GetWidth() / 2 - 130, 80, 80 }, this);
	btn10 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "Test10", { (app->win->GetWidth() / 2) + 280 , app->win->GetWidth() / 2 -300, 160, 80 }, this);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}
bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{

	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		//Checks the GUI element ID
		if (control->id == 1)
		{
			play = true;
			LOG("Click on button 1");
		}

		if (control->id == 2)
		{
			credits = true;
			LOG("Click on button 2");
		}
		if (control->id == 3)
		{
			settings = true;
			LOG("Click on button 3");
		}

		if (control->id == 4)
		{
			exit = true;
			LOG("Click on button 4");
		}
		
		if (control->id == 5)
		{
			musicon = true;
			LOG("Click on button 5");
		}
		
		if (control->id == 6)
		{
			musiconfx = true;
			LOG("Click on button 6");
		}
		
		if (control->id == 7)
		{
			fullscreen = true;
			LOG("Click on button 7");
		}
		if (control->id == 8)
		{
			vsyncon = true;
			LOG("Click on button 8");
		}
		if (control->id == 9)
		{
			returnto = true;
			LOG("Click on button 9");
		}
		if (control->id == 10)
		{
			continuar = true;
			LOG("Click on button 10");
		}
	}
	//Other cases here
	
	default: break;
	}

	return true;
}
// Called each loop iteration
bool Scene::Update(float dt)
{


	
	


	
	
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
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN&&app->checkp->check==true) {

		app->player->position.x = app->checkp->Cposition2.x;
		app->player->position.y = app->checkp->Cposition2.y;
	}
	

	//Checkpoints
	Checkpoints();

	//Teleport
	Teleports();

	StartChTpColliders();

	if (app->Title->logo ==false)
	{
		
		Menu = true;
	}
	if (play == true) {
		Menu = false;
		app->Title->Intro = false;
	}
	
	// Draw ma
	app->map->Draw();
	/*if (app->Title->logo == true)
	{*/
		app->guiManager->Draw();
	//}
		/*sprintf_s(timer, 6, "%03d", (int)timerValue);
		app->fonts->BlitText(app->player->position.x, app->player->position.y - offsetTimer - 12, cameraPos.y + 2, font, timerTitle.GetString());
		app->fonts->BlitText(app->player->position.x, app->player->position.y - (36 * 3) - 30, cameraPos.y + 2, font, ":");
		app->fonts->BlitText(app->player->position.x, app->player->position.y - (36 * 3), cameraPos.y + 2, font, timer);
	*/

		
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	
	bool ret = true;
	
	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN||app->Title->salir==true)
		ret = false;
	if (app->player->vidas == 2 && app->Title->logo == false && app->Title->Intro == false) {

		app->render->DrawTexture(texture2, app->corazon->position.x + 72, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);
	}

	if (app->player->vidas <= 1 && app->Title->logo == false && app->Title->Intro == false) {

		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);

	}



	if (app->player->vidas >= 3 && app->Title->logo == false && app->Title->Intro == false) {
		app->render->DrawTexture(texture, app->corazon->position.x, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture2, app->corazon->position.x + 72, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(texture3, app->corazon->position.x + 144, app->corazon->position.y, 0, 0, 0);
	}

	if (app->coin->moneda == 1 && app->Title->logo == false && app->Title->Intro == false) {
		app->render->DrawTexture(Moneda, app->corazon->position.x + 250, app->corazon->position.y, 0, 0, 0);
	}
	if (app->coin->moneda == 2 && app->Title->logo == false && app->Title->Intro == false) {
		app->render->DrawTexture(Moneda, app->corazon->position.x + 250, app->corazon->position.y, 0, 0, 0);
		app->render->DrawTexture(Moneda, app->corazon->position.x + 300, app->corazon->position.y, 0, 0, 0);
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
	/*CheckP = app->physics->AddCollider({ 70, 260, 20,20 }, Collider::Type::CHECKPOINT, this);
	CheckP2 = app->physics->AddCollider({ 233, 28, 20,20 }, Collider::Type::CHECKPOINT2, this);*/

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
