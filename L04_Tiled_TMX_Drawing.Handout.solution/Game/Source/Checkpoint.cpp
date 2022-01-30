#include "Checkpoint.h"
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
#include"SceneTitle.h"
#include "Map.h"
//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

Checkpoint::Checkpoint() : Module()
{


}



Checkpoint::~Checkpoint()
{
}







bool Checkpoint::Start()
{
	LOG("Loading player textures");

	bool ret = true;


	
	checpoint = app->audio->LoadFx("Assets/fx/chec.wav");
	textureC = app->tex->Load("Assets/Sprites/bandoleiro1.png");
	textureC2 = app->tex->Load("Assets/Sprites/bandoleiro2.png");

	// Posición inicial (depende del lvl)


	// X, Y, anchura, altura, 
	//checkpoint1 = app->physics->AddCollider({ Cposition.x, Cposition.y, 115, 171 }, Collider::Type::CHECKPOINT, this);
	checkpoint2 = app->physics->AddCollider({ Cposition2.x, Cposition2.y, 115, 171 }, Collider::Type::CHECKPOINT2, this);
	/*Cposition.x = 0;
	Cposition.y = 999;*/
	Cposition2.x = 6400;
	Cposition2.y = 1400;
	position.x = 25;
	position.y = 25;
	return ret;
}


bool Checkpoint::Update(float dt)
{






	//checkpoint1->SetPos(Cposition.x, Cposition.y);
	checkpoint2->SetPos(Cposition2.x, Cposition2.y);

	if (print == false && print2 == false&& app->Title->logo == false && app->Title->Intro == false) {
		//app->render->DrawTexture(textureC, Cposition.x, Cposition.y);
		app->render->DrawTexture(textureC, Cposition2.x, Cposition2.y);
	}
	
	//godmode
	if (godmode == true) {
		collision = false;
		collision2 = false;


	}

	/*if (collision == true) {
		print = true;
		musica = true;
		if (musica == true) {
			app->audio->PlayFx(corazonFx);

		}
		
	}

	if (print == true) {

		app->render->DrawTexture(textureC2, Cposition.x, Cposition.y);


	}*/
	if (collision2 == true&&cont!=1) {
		print2 = true;
		musica = true;
		cont++;
		if (musica == true) {
			app->audio->PlayFx(checpoint);

		}
		check = true;
	}
	if (print2 == true&& app->Title->logo == false && app->Title->Intro == false) {

		app->render->DrawTexture(textureC2, Cposition2.x, Cposition2.y);


	}
	
	//collision = false;
	collision2 = false;
	
	musica = false;
	return true;
}

bool Checkpoint::PostUpdate()
{







	return true;
}

void Checkpoint::OnCollision(Collider* c1, Collider* c2)
{

	

}

bool Checkpoint::loadState(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	// pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0.0f);
	return true;
}


bool Checkpoint::saveState(pugi::xml_node& data) const
{
	data.child("position").attribute("x").set_value(position.x);
	data.child("position").attribute("y").set_value(position.y);
	return true;
}
