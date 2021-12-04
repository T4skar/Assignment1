#include "ModulePlayer.h"
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

ModulePlayer::ModulePlayer() : Module()
{

	// Aqui van las animaciones del player
	idleRightAnim.PushBack({ 1050,670,110,170 });
	idleRightAnim.PushBack({ 1164,670,110,170 });
	idleRightAnim.PushBack({ 1278,670,110,170 });
	idleRightAnim.PushBack({ 1400,670,110,170 });
	idleRightAnim.loop = true;
	idleRightAnim.speed = 0.003f;

	rightAnim.PushBack({ 1657,687,110,170 });
	rightAnim.PushBack({ 1790,682,110,170 });
	rightAnim.PushBack({ 1922,682,110,170 });
	rightAnim.PushBack({ 2060,682,110,170 });
	rightAnim.PushBack({ 2187,682,110,170 });
	rightAnim.PushBack({ 2315,682,110,170 });
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
	

	// Posici�n inicial (depende del lvl)

	 
	// X, Y, anchura, altura, 
	collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::PLAYER, this);
	right = app->physics->AddCollider({ position.x, position.y, 8, 80 }, Collider::Type::RIGHT, this);
	left = app->physics->AddCollider({ position.x, position.y, 24, 80 }, Collider::Type::LEFT, this);
	up = app->physics->AddCollider({ position.x, position.y, 24, 24 }, Collider::Type::UP, this);
	
	return ret;
}


bool ModulePlayer::Update(float dt)
{
	/*active = false;*/
	collider->SetPos(position.x, position.y - 14);
	right->SetPos(position.x+107, position.y);
	left->SetPos(position.x, position.y);
	up->SetPos(position.x, position.y);

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
		position.y += 2;
	}


	//se activa la gravedad si ya no colisiona
	if (collision == false) {
		gravity = true;	
	}

	

	//pantalla de lose
	if (dead == true) {

		app->render->DrawTexture(app->scene->lose, app->render->camera.x, app->render->camera.y);
	}


	//no avanzar horizontalmente
	

	//pantalla victoria
	if (Win == true) {
		app->render->DrawTexture(app->scene->Win, app->render->camera.x, app->render->camera.y);
	}





		if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)&&Right==false) {		

			position.x += 1;

			if (currentAnimation != &rightAnim) {
				rightAnim.Reset();
				currentAnimation = &rightAnim;

			}


		}
		else if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) && (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)&&left==false) {
			jump = true;
			if (jump == true) {
				position.y -= 80;
				position.x -= 3;
			}
			if (currentAnimation == &upAnim) {
				if (currentAnimation != &idleUpAnim) {
					idleUpAnim.Reset();
					currentAnimation = &idleUpAnim;
				}
			}

		}

		else if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)) {
			jump = true;


			if (currentAnimation != &upAnim) {
				upAnim.Reset();
				currentAnimation = &upAnim;
			}
			if (jump == true) {

				position.y -= 150;
				
				gravity = true;
			}
		}
		else if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) && (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)){
			jump = true;
			if (jump == true) {
				position.y -= 80;
				position.x -= 3;
			}
			if (currentAnimation == &upAnim) {
				if (currentAnimation != &idleUpAnim) {
					idleUpAnim.Reset();
					currentAnimation = &idleUpAnim;
				}
			}

		}

		else if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)&&Left==false) {		// mov izquierda

			position.x -= 1;


			if (currentAnimation != &leftAnim) {
				leftAnim.Reset();
				currentAnimation = &leftAnim;
			}


		}

	

		//if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && nPosX == 0 && nPosY == 0)){		// mov abajo
		//	nPosY = position.y - 1;
		//}


		else {

			if (currentAnimation != &idleRightAnim) {
				idleRightAnim.Reset();
				currentAnimation = &idleRightAnim;
			}
		}

		// player stop the animation when stop walking




		
	
		

	currentAnimation->Update();

	return true;
}

 bool ModulePlayer::PostUpdate()
{
	
	
	 SDL_Rect rect = currentAnimation->GetCurrentFrame();
	/* rect.h = 1035;
	 rect.w = 1194;
	 rect.x = 42;
	 rect.y = 55;*/
		app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,0,0, 0);
		
	


	return true;
}

 void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
 {
	
		 if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::GROUND) {
			 
			// app->player->position.y = 3198;
			 collision = true;
			 gravity = false;
			 
		 }
		
		 if (c1->type == Collider::Type::RIGHT && c2->type == Collider::Type::GROUND) {
			 Right = true;
		 }

		 if (c1->type == Collider::Type::LEFT && c2->type == Collider::Type::GROUND) {
			 Left=true;
		 }
		 
	 
		if (c1->type == Collider::Type::PLAYER && c2->type == Collider::Type::DEAD) {

		 dead = true;
		 
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


 void ModulePlayer::set_camera()
 {
	 //Center the camera over the dot
	 app->render->camera.x = (position.x + 127 / 2) - 30 / 2;
	 app->render->camera.y = (position.y + 170 / 2) - 30 / 2;

	 //Keep the camera in bounds.
	 if (app->render->camera.x < 0)
	 {
		 app->render->camera.x = 0;
	 }
	 if (app->render->camera.y < 0)
	 {
		 app->render->camera.y = 0;
	 }
	 if (app->render->camera.x > app->map->mapData.tileWidth - app->render->camera.w)
	 {
		 app->render->camera.x = app->map->mapData.tileWidth - app->render->camera.w;
	 }
	 if (app->render->camera.y > app->map->mapData.tileHeight - app->render->camera.h)
	 {
		 app->render->camera.y = app->map->mapData.tileHeight - app->render->camera.h;
	 }
 }


 












