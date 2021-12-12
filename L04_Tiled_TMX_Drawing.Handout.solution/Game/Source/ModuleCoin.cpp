#include "ModuleCoin.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include "Defs.h"
#include "Log.h"
#include "Animation.h"
#include"ModulePlayer.h"
#include "Module.h"
#include "ModulePhysics.h"
#include "Map.h"
//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

ModuleCoin::ModuleCoin() : Module()
{

	
}



ModuleCoin::~ModuleCoin()
{
}







bool ModuleCoin::Start()
{
	LOG("Loading player textures");

	bool ret = true;


	corazonFx = app->audio->LoadFx("Assets/fx/salto.wav");


	// Posici�n inicial (depende del lvl)
	coin = app->physics->AddCollider({ Cposition.x, Cposition.y, 115, 171 }, Collider::Type::COIN2, this);
	COposition.x = 4200;
	COposition.y = 1500;
	// X, Y, anchura, altura, 
	coin = app->physics->AddCollider({ Cposition.x, Cposition.y, 115, 171 }, Collider::Type::COIN, this);
	Cposition.x = 4500;
	Cposition.y = 1500;
	position.x = 25;
	position.y = 25;
	return ret;
}


bool ModuleCoin::Update(float dt)
{
	
	collision = false;



	coin->SetPos(Cposition.x, Cposition.y);


	//godmode
	if (godmode == true) {
		collision = false;


	}

	if (collision == true) {
		app->audio->PlayFx(corazonFx);
	}

	return true;
}

bool ModuleCoin::PostUpdate()
{








	return true;
}

void ModuleCoin::OnCollision(Collider* c1, Collider* c2)
{

	if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::COIN) {
		

		collision = true;


	}
	if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::COIN2) {


		collision = true;


	}
}

bool ModuleCoin::loadState(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	// pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0.0f);
	return true;
}


bool ModuleCoin::saveState(pugi::xml_node& data) const
{
	data.child("position").attribute("x").set_value(position.x);
	data.child("position").attribute("y").set_value(position.y);
	return true;
}


 












