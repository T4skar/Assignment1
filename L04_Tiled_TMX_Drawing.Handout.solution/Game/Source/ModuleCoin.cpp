#include "ModuleCoin.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include"Scene.h"
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

ModuleCoin::ModuleCoin(int x, int y) : Entity(x, y, EntityType::COIN)
{

	
}



ModuleCoin::~ModuleCoin()
{
}







bool ModuleCoin::Start()
{
	LOG("Loading player textures");

	bool ret = true;


	corazonFx = app->audio->LoadFx("Assets/audio/fx/coin.wav");

	textureC = app->tex->Load("Assets/Sprites/coin.png");
	textureC2 = app->tex->Load("Assets/Sprites/coin.png");
	//texturecora = app->tex->Load("Assets/Sprites/corazones.png");
	// Posición inicial (depende del lvl)
	coin2 = app->physics->AddCollider({ COposition.x, COposition.y, 115, 171 }, Collider::Type::COIN2, this);
	COposition.x = 4350;
	COposition.y = 1500;
	// X, Y, anchura, altura, 
	coin = app->physics->AddCollider({ Cposition.x, Cposition.y, 115, 171 }, Collider::Type::COIN);
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
	coin2->SetPos(COposition.x, COposition.y);


	//godmode
	if (godmode == true) {
		collision = false;


	}

	if (collision == true) {
		print = true;
		musica = true;
		if (musica == true) {
			app->audio->PlayFx(corazonFx);

		}
		Cposition.x = 10000;
		Cposition.y = 15000;
	}
	
	if (collision2 == true) {
		print2 = true;
		musica = true;
		if (musica == true) {
			app->audio->PlayFx(corazonFx);

		}
		COposition.x = 10000;
		COposition.y = 15000;
		
	}
	/*if (app->corazon->print == false) {
		app->render->DrawTexture(texturecora, app->corazon->Cposition.x, app->corazon->Cposition.y);
	}*/
	if (print == false) {
		app->render->DrawTexture(textureC,Cposition.x, Cposition.y);
		
	}
	if (print == false) {
		app->render->DrawTexture(textureC2, COposition.x, COposition.y);
	}
	musica = false;

	return true;
	
}

bool ModuleCoin::PostUpdate()
{
	
	collision2 = false;

	collision = false;

	




	return true;
}

void ModuleCoin::OnCollision(Collider* c1, Collider* c2)
{

	
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


 












