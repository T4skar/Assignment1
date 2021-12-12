#include "ModulePlayer.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include "Defs.h"
#include "Log.h"
#include "Animation.h"
#include "ModuleCoin.h"

#include"Corazones.h"
#include "Module.h"
#include "ModulePhysics.h"
#include "Map.h"
#include "Checkpoint.h"
//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

ModulePlayer::ModulePlayer() : Module()
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



ModulePlayer::~ModulePlayer()
{
}







bool ModulePlayer::Start()
{
	LOG("Loading player textures");

	bool ret = true;
	
	texture = app->tex->Load("Assets/Sprites/Natsu3.png");
	
	
	currentAnimation = &idleRightAnim;
	
	
	// Posición inicial (depende del lvl)
	
	 
	// X, Y, anchura, altura, 
	collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::PLAYER, this);
	right = app->physics->AddCollider({ position.x, position.y, 8, 80 }, Collider::Type::RIGHT, this);
	left = app->physics->AddCollider({ position.x, position.y, 24, 80 }, Collider::Type::LEFT, this);
	up = app->physics->AddCollider({ position.x, position.y, 24, 24 }, Collider::Type::UP, this);
	matar = app->physics->AddCollider({ position.x, position.y, 115, 24 }, Collider::Type::MATAR, this);
	return ret;
}


bool ModulePlayer::Update(float dt)
{

	float speed = 1.0f*dt*0.9;
	collision = false;
	
	
	
	collider->SetPos(position.x, position.y - 14);
	right->SetPos(position.x+107, position.y);
	left->SetPos(position.x, position.y);
	up->SetPos(position.x, position.y);
	matar->SetPos(position.x, position.y + 160);
	//godmode
	if(godmode==true){
		collision = false;
		gravity = false;
		Right = true;
		Left = true;
		if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)) {
			position.y += speed;
		}
		if ((app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)) {
			position.y -= speed;
		}
		if (playingM==false) {
			playingM = true;
			app->scene->playMusic = false;
		}
		
	}
	else {
		
		playingM = false;
	}
	

	//desactivacion gravedad
	if (gravity == false) {
		position.y += 0;
	}


	//colision con parte superior plataformas
	if (collision == true) {
		gravity = false;
	}


	//activacion gravedad
	if (gravity == true  ) {
		position.y += 2*speed*0.40;
	}


	//se activa la gravedad si ya no colisiona
	if (collision == false) {
		gravity = true;	
	} 

	

	//pantalla de lose
	if (dead == true) {
		app->scene->playMusic = false;
		
		
		if (vidas== 0) {
			app->render->camera.x = 0;
			app->render->camera.y = 0;

			app->render->DrawTexture(app->scene->lose, 200, 100);
		}
		else {
			
			app->scene->playMusic = false;
			dead = false;
			vidas--;

			position.x = 250;
			position.y = 760;
		}
		 
		
		
	}
	
	//pantalla victoria
	if (Win == true) {
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->render->DrawTexture(app->scene->Win, 200, 100);
	}

	
	 //COLISIONES HORIZONTALES
	if (Right == false) {
		position.x -= 1;
	}
	if (Left == false) {
		position.x += 1;
	}
	
	   if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) && Right == true) {

		position.y -= 500;
		position.x += 80;


		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}

	}
	  
	else if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && Right == true) {		// mov izquierda

		position.x += speed;


		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}


	 }
	

	  else if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)) {
		  
				position.y -= 500 ;
				salto = true;
				gravity = true;
				

			if (currentAnimation != &upAnim) {
				upAnim.Reset();
				currentAnimation = &upAnim;
			}
			
			
	  }
	  else if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) && (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) && Left == true){
			
				position.y -= 500;
				position.x -= 80;
			
			if (currentAnimation == &upAnim) {
				if (currentAnimation != &idleUpAnim) {
					idleUpAnim.Reset();
					currentAnimation = &idleUpAnim;
				}
			}

	  }
	
	  else if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) && Left ==true) {		// mov izquierda

			position.x -= speed;


			if (currentAnimation != &leftAnim) {
				leftAnim.Reset();
				currentAnimation = &leftAnim;
			}


	   }
	  else if ((app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)) {
		   if (currentAnimation == &idleRightAnim) {
			   if (currentAnimation != &atackRightAnim) {
				   atackRightAnim.Reset();
				   currentAnimation = &atackRightAnim;
			   }
		   }
		  
	   }
	    
	else {

		if (currentAnimation != &idleRightAnim) {
				idleRightAnim.Reset();
				currentAnimation = &idleRightAnim;
		}
	}

		// player stop the animation when stop walking 



	 
		
	
	   Right = true;
	   Left = true;

	currentAnimation->Update();
	if ((app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)) {
		godmode = true;
		
	}
	if ((app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)) {
		godmode = false;
		app->scene->playMusic = false;
		dead = false;
		Win = false;
		app->corazon->print = false;
		app->coin->print = false;
		app->coin->print2 = false;
		vidas=4;
		position.x = 250;
		position.y = 760; 

	}
	return true;
}

 bool ModulePlayer::PostUpdate()
{
	
	
	 SDL_Rect rect = currentAnimation->GetCurrentFrame();
	
		app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,0,0, 0);
		
	


	return true;
}

 void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
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
		
		// vida = false;
		// vidas--;
		 
		}
		
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::WIN) {

		Win = true;
		 gravity = false;
		 //vida = true;
		

		 
	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::CHECKPOINT)
	 {
		 if (app->scene->Point == false) {
			 app->audio->PlayFx(GetCheckpoint);
		 }
		 app->scene->Point = true;
		 CheckActive = true;
		 app->scene->ActiveTeleport = true;
		 app->scene->tps == 1;
	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::CHECKPOINT2)
	 {
		 if (app->scene->Point2 == false) {
			 app->audio->PlayFx(GetCheckpoint);
		 }
		 app->scene->Point2 = true;
		 CheckActive2 = true;
		 app->scene->ActiveTeleport2 = true;
		 app->scene->tps2 == 1;
	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::CORAZON) {


		app->corazon-> collision = true;
		vidas +=3;


	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::COIN) {


		app->coin-> collision = true;
		 app->coin->print = true;


	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::COIN2) {


		 app->coin->collision2 = true;
		app->coin-> print2 = true;

	 }
	 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::ENEMY)
	 {
		 vidas--;
		 dead = true;
	 }
 }

 bool ModulePlayer::loadState(pugi::xml_node& data)
 {
	 position.x = data.child("position").attribute("x").as_int();
	 position.y = data.child("position").attribute("y").as_int();
	// pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0.0f);
	 return true;
 }


 bool ModulePlayer::saveState(pugi::xml_node& data) const
 {
	 data.child("position").attribute("x").set_value(position.x);
	 data.child("position").attribute("y").set_value(position.y);
	 return true;
 }



 












