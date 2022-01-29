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

	atackRightAnim.PushBack({ 907, 1272, 110, 170 });
	atackRightAnim.PushBack({ 1243, 1272, 110, 170 });
	atackRightAnim.PushBack({ 1414, 1273, 110, 170 });
	atackRightAnim.PushBack({ 1567, 1275, 110, 170 });

	atackRightAnim.loop = true;
	atackRightAnim.speed = 0.002f;
}



ModuleEnemyVolador::~ModuleEnemyVolador()
{
}


bool ModuleEnemyVolador::Start()
{
	LOG("Loading enemy textures");

	bool ret = true;
	
	texture = app->tex->Load("Assets/Sprites/Natsu3.png");

	position.x = 1500;
	position.y = 1400;
	// X, Y, anchura, altura, 
	//collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMY, this);

	colliderE = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMYV, this);
	enemyRight = app->physics->AddCollider({ position.x, position.y, 500, 161 }, Collider::Type::ENEMYVR, this);
	enemyLeft = app->physics->AddCollider({ position.x, position.y, -500, 161 }, Collider::Type::ENEMYVL, this);
	enemyUp = app->physics->AddCollider({ position.x, position.y, 1000, 500 }, Collider::Type::ENEMYUP, this);
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
	enemyUp->SetPos(position.x - 500, position.y +275);
	enemyDown->SetPos(position.x - 500, position.y -100 );
	
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

	
	if (eleft == true) {
		position.x -= speed;
		/*if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}*/
	}
	if (eright == true) {
		position.x += speed;

		/*if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}*/
	}
	if (eup == true) {
		position.y += speed;

	}
	if (edown == true) {
		position.y -= speed;

	}
	else {

		if (currentAnimation != &idleRightAnim) {
			idleRightAnim.Reset();
			currentAnimation = &idleRightAnim;
		}
	}

	currentAnimation->Update();
	dead = false;
	eright = false;
	eleft = false;
	eup = false;
	edown = false;
	
	
	return true;
}



bool ModuleEnemyVolador::PostUpdate()
{

	/*SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, 0, 0, 0);*/






	return true;
}

void ModuleEnemyVolador::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == Collider::Type::ENEMYV && c2->type == Collider::Type::GROUND) {


		collision = true;
		

	}if (c1->type == Collider::Type::ENEMYV && c2->type == Collider::Type::PLAYER) {


		dead = true;

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

	}
	if (c1->type == Collider::Type::ENEMYDOWN && c2->type == Collider::Type::PLAYER) {



		edown = true;

	}
	
}