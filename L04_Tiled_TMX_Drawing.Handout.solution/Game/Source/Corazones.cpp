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
	
	
	
	

	// Posición inicial (depende del lvl)
	
	 
	// X, Y, anchura, altura, 
	cora = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::CORAZON, this);
	position.x = 25;
	position.y = 25;
	return ret;
}


bool Corazones::Update(float dt)
{
	

	collision = false;
	
	
	
	cora->SetPos(25,25);


	//godmode
	if(godmode==true){
		collision = false;
	
		
	}
	
	

	if ((app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)) {
		godmode = true;
		
	}
	if ((app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)) {
		godmode = false;
		

	}
	return true;
}

 bool Corazones::PostUpdate()
{
	
	
	 
		
		
	


	return true;
}

 void Corazones::OnCollision(Collider* c1, Collider* c2)
 {
	
		 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::CORAZON) {
			 
			
			 collision = true;
			 gravity = false;
			 
		 }
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



 












