#include "Checkpoint.h"

#include "App.h"
#include "Textures.h"
#include "Animation.h"
#include "Input.h"
#include "Render.h"
#include "Window.h"
#include "Audio.h"
#include "Collisions.h"
#include "Scene.h"
#include "Player.h"
#include "PathFinding.h"

#include "Map.h"
#include "Defs.h"
#include "Log.h"



/*
Checkpoint::CheckPo() : Module()
{
	RedFlag.PushBack({ 119, 5, 76, 128 });

	GreenFlag.PushBack({ 13, 5, 76, 128 });
}

Checkpoint::~CheckPo()
{
}
*/

bool Checkpoint::Start()
{
	bool ret = true;
	Chpoint = app->tex->Load("Assets/textures/bandoleiro.png");
	currentAnimation = &RedFlag;

	Chpointcoll = app->physics->AddCollider({ PChpoint.x - 5,PChpoint.y - 650, 80 ,800 }, Collider::Type::CHECKPOINT, this);
	return ret;
}

bool Checkpoint::Update(float dt)
{
	if (checkpoint == false)
	{
		currentAnimation = &RedFlag;
	}

	if (checkpoint == true)
	{
		if (FlagGreen == false)
		{
			currentAnimation = &GreenFlag;
			FlagGreen = true;
			saved = true;
		}

	}

	if (saved == true)
	{
		app->SaveGameRequest();
		saved = false;
	}

	return true;
}

bool Checkpoint::PostUpdate()
{

	/*//draw 
	Asaverect = currentAnimation->GetCurrentFrame();

	if (app->scene->DeadScreen == false && app->scene->WScrean == false && app->scene->EnterScreen == false && autosave == false)
	{
		app->render->DrawTexture(Asave, PAsave.x, PAsave.y, &Asaverect);
	}

	if (app->scene->DeadScreen == false && app->scene->WScrean == false && app->scene->EnterScreen == false && autosave == true)
	{
		app->render->DrawTexture(Asave, PAsave.x, PAsave.y, &Asaverect);
	}
	currentAnimation->Update();

	return true;
}*/
