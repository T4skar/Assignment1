#include "Corazones.h"
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

Corazones::Corazones() : Module()
{

	
}



Corazones::~Corazones()
{
}







bool Corazones::Start()
{
	LOG("Loading player textures");

	bool ret = true;
	
	
	corazonFx = app->audio->LoadFx("Assets/fx/salto.wav");
	textureC = app->tex->Load("Assets/Sprites/corazones.png");

	// Posición inicial (depende del lvl)
	
	 
	// X, Y, anchura, altura, 
	cora = app->physics->AddCollider({ Cposition.x, Cposition.y, 115, 171 }, Collider::Type::CORAZON, this);
	Cposition.x = 7180;
	Cposition.y = 999;
	position.x = 25;
	position.y = 25;
	return ret;
}


bool Corazones::Update(float dt)
{
	
	
	
	
	
	
	cora->SetPos(Cposition.x,Cposition.y);


	//godmode
	if(godmode==true){
		collision = false;
	
		
	}
	
	if (collision == true) {
		print = true;
		app->audio->PlayFx(corazonFx);
	}

	if (print == false) {
		
			app->render->DrawTexture(textureC, Cposition.x, Cposition.y);

		
	}
	
	return true;
}

 bool Corazones::PostUpdate()
{
	
	 
		
		
	


	return true;
}

 void Corazones::OnCollision(Collider* c1, Collider* c2)
 {
	
		
 }

 bool Corazones::loadState(pugi::xml_node& data)
 {
	 position.x = data.child("position").attribute("x").as_int();
	 position.y = data.child("position").attribute("y").as_int();
	// pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0.0f);
	 return true;
 }


 bool Corazones::saveState(pugi::xml_node& data) const
 {
	 data.child("position").attribute("x").set_value(position.x);
	 data.child("position").attribute("y").set_value(position.y);
	 return true;
 }



 












