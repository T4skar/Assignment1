#include "Corazones.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include "Defs.h"
#include "Log.h"
#include "Animation.h"
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

	// Aqui van las animaciones del player
	idleRightAnim.PushBack({ 1050,670,110,170 });
	idleRightAnim.PushBack({ 1164,670,110,170 });
	idleRightAnim.PushBack({ 1278,670,110,170 });
	idleRightAnim.PushBack({ 1400,670,110,170 });
	idleRightAnim.loop = true;
	idleRightAnim.speed = 0.003f;

	rightAnim.PushBack({ 1662,687,110,170 });
	rightAnim.PushBack({ 1795,682,110,170 });
	rightAnim.PushBack({ 1927,682,110,170 });
	rightAnim.PushBack({ 2065,682,110,170 });
	rightAnim.PushBack({ 2192,682,110,170 });
	rightAnim.PushBack({ 2320,682,110,170 });
	rightAnim.loop = true;
	rightAnim.speed = 0.02f;

	upAnim.PushBack({ 52, 924,110,170 });
	upAnim.PushBack({ 188,887,110,170 });
	upAnim.PushBack({ 309,920,110,170 });
	upAnim.PushBack({ 482,893,110,170 });
	upAnim.loop = true;
	upAnim.speed = 0.02f;

	leftAnim.PushBack({ 1961,1381,110,170 });
	leftAnim.PushBack({ 2094,1381,110,170 });
	leftAnim.PushBack({ 2215,1381,110,170 });
	leftAnim.PushBack({ 2355,1381,110,170 });
	leftAnim.PushBack({ 2489,1381,110,170 });
	leftAnim.PushBack({ 2625,1381,110,170 });
	leftAnim.loop = true;
	leftAnim.speed = 0.02f;

	atackRightAnim.PushBack({907, 1272, 110, 170});
	atackRightAnim.PushBack({1243, 1272, 110, 170});
	atackRightAnim.PushBack({1414, 1273, 110, 170});
	atackRightAnim.PushBack({1567, 1275, 110, 170});
	atackRightAnim.loop = true;
	atackRightAnim.speed = 0.02f;
}



Corazones::~Corazones()
{
}







bool Corazones::Start()
{
	LOG("Loading player textures");

	bool ret = true;
	
	texture = app->tex->Load("Assets/Sprites/corazones.png");
	
	
	currentAnimation = &idleRightAnim;
	

	// Posición inicial (depende del lvl)
	
	 
	// X, Y, anchura, altura, 
	collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::PLAYER, this);
	right = app->physics->AddCollider({ position.x, position.y, 8, 80 }, Collider::Type::RIGHT, this);
	left = app->physics->AddCollider({ position.x, position.y, 24, 80 }, Collider::Type::LEFT, this);
	up = app->physics->AddCollider({ position.x, position.y, 24, 24 }, Collider::Type::UP, this);
	
	return ret;
}


bool Corazones::Update(float dt)
{


	collision = false;
	
	
	
	collider->SetPos(position.x, position.y - 14);
	right->SetPos(position.x+107, position.y);
	left->SetPos(position.x, position.y);
	up->SetPos(position.x, position.y);

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
	
	
	 SDL_Rect rect = currentAnimation->GetCurrentFrame();
	
		app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,0,0, 0);
		
	


	return true;
}

 void Corazones::OnCollision(Collider* c1, Collider* c2)
 {
	
		 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::GROUND) {
			 
			
			 collision = true;
			 gravity = false;
			 
		 }
		
		 if (c1->type == Collider::Type::RIGHT && c2->type == Collider::Type::GROUND) {
			 Right = false;
			
		 }
		 

		 if (c1->type == Collider::Type::LEFT && c2->type == Collider::Type::GROUND) {
			 Left = false;
			 
		 }
		 
	 
		if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::DEAD) {

		 dead = true;
		 gravity = false;
		// vida = false;
		 
		}
		if (c1->type == Collider::Type::UP && c2->type == Collider::Type::GROUND) {

			
			//aqui va el bumper
			
			
		}
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::WIN) {

		Win = true;
		 gravity = false;
		 //vida = true;
		

		 
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



 












