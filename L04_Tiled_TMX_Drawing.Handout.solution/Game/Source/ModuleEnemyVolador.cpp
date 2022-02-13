#include "ModuleEnemyVolador.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include"ModulePlayer.h"
#include "Defs.h"
#include "Log.h"
#include "Animation.h"
#include "Module.h"
#include"SceneTitle.h"
#include "ModulePhysics.h"
#include "Map.h"
//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

ModuleEnemyVolador::ModuleEnemyVolador(bool startEnabled) : Module()
{

	// Aqui van las animaciones del player


	rightAnim.PushBack({ 29,40,110,104 });
	rightAnim.PushBack({ 230,44,110,104 });
	rightAnim.PushBack({ 381,46,110,104 });

	rightAnim.loop = true;
	rightAnim.speed = 0.1f;

	

	

	


}



ModuleEnemyVolador::~ModuleEnemyVolador()
{
}


bool ModuleEnemyVolador::Start()
{
	LOG("Loading enemy textures");

	bool ret = true;
	
	texture = app->tex->Load("Assets/Sprites/happy.png");

	position.x = 2500;
	position.y = 1450;
	// X, Y, anchura, altura, 
	//collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMY, this);

	colliderE = app->physics->AddCollider({ position.x, position.y, 110, 110 }, Collider::Type::ENEMYV, this);
	enemyRight = app->physics->AddCollider({ position.x, position.y, 1000, 400 }, Collider::Type::ENEMYVR, this);
	enemyLeft = app->physics->AddCollider({ position.x, position.y, -500, 161 }, Collider::Type::ENEMYVL, this);
	enemyUp = app->physics->AddCollider({ position.x, position.y, 1200, 600 }, Collider::Type::ENEMYUP, this);
	enemyDown = app->physics->AddCollider({ position.x, position.y, 1000, -500 }, Collider::Type::ENEMYDOWN, this);
	return ret;
}

bool ModuleEnemyVolador::Update(float dt)
{
	float speed = 1.0f * dt * 0.3;
	//collision = false;

	colliderE->SetPos(position.x, position.y);
	enemyRight->SetPos(position.x + 115, position.y);
	enemyLeft->SetPos(position.x , position.y);
	enemyUp->SetPos(position.x - 500, position.y -500);
	enemyDown->SetPos(position.x - 490, position.y + 575);
	
	if (dead == true)
	{
		
		eright = false;
		eleft = false;
		eup = false;
		edown = false;
		position.y = 11000;
		position.x = 11000;

		/*if (currentAnimation != &dieAnim) {
			dieAnim.Reset();
			currentAnimation = &dieAnim;
		}*/

	}
	else
	{
		collision = false;
	}
	
	/*if (collision = true)
	{
		position.y -= speed;
	}*/

	
	//if (eleft == true) {
	//	position.x -= speed;
	//	/*if (currentAnimation != &rightAnim) {
	//		rightAnim.Reset();
	//		currentAnimation = &rightAnim;
	//	}*/
	//}
	if (eright == true&& app->Title->logo == false && app->Title->Intro == false){
		position.x += speed;
		position.y += speed;
		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}
	if (eup == true && app->Title->logo == false && app->Title->Intro == false) {
		position.y -= speed; 
		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}
	if (abajo == true&&app->Title->logo == false && app->Title->Intro == false){
		position.y += speed;
		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}
	else {

		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}

	currentAnimation->Update();
	dead = false;
	eright = false;
	eleft = false;
	eup = false;
	abajo = false;
	
	
	return true;
}



bool ModuleEnemyVolador::PostUpdate()
{
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	if (app->Title->logo == false && app->Title->Intro == false) {
		

		app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, 0, 0, 0);
	}
	/*SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, 0, 0, 0);*/






	return true;
}

void ModuleEnemyVolador::OnCollision(Collider* c1, Collider* c2)
{
	
	if (c1->type == Collider::Type::ENEMYV && c2->type == Collider::Type::GROUND) {


		collision = true;
		

	}if (c1->type == Collider::Type::ENEMYV && c2->type == Collider::Type::PLAYER) {


		app->player->dead = true;

	}
	if (c1->type == Collider::Type::ENEMYV && c2->type == Collider::Type::MATAR) {


		dead = true;

	}
	if (c1->type == Collider::Type::ENEMYVL && c2->type == Collider::Type::PLAYER) {



		eleft = true;

	}
	if (c1->type == Collider::Type::ENEMYVR && c2->type == Collider::Type::PLAYER) {



		eright = true;

	}
	if (c1->type == Collider::Type::ENEMYUP && c2->type == Collider::Type::PLAYER) {


		eup = true;
		//abajo = true;
	}
	if (c1->type == Collider::Type::ENEMYDOWN && c2->type == Collider::Type::PLAYER) {


		abajo = true;

	}
	
	
}