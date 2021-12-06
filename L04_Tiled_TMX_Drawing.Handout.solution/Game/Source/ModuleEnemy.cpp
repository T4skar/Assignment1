#include "ModuleEnemy.h"
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

ModuleEnemy::ModuleEnemy() : Module()
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



ModuleEnemy::~ModuleEnemy()
{
}


bool ModuleEnemy::Start()
{
	LOG("Loading enemy textures");

	bool ret = true;
	
	texture = app->tex->Load("Assets/Sprites/Natsu3.png");


	position.x = 500;
	position.y = 760;
	// X, Y, anchura, altura, 
	//collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMY, this);

	colliderE = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMY, this);
	enemyRight = app->physics->AddCollider({ position.x, position.y, 500, 80 }, Collider::Type::ENEMYR, this);
	enemyLeft = app->physics->AddCollider({ position.x, position.y, -500, 80 }, Collider::Type::ENEMYL, this);
	return ret;
}

bool ModuleEnemy::Update(float dt)
{
	collision = false;

	colliderE->SetPos(position.x, position.y - 14);
	enemyRight->SetPos(position.x, position.y - 14);
	enemyLeft->SetPos(position.x, position.y - 14);

	if (gravity == false) {
		position.y += 0;
	}

	if (gravity == true) {
		position.y += 2;
	}

	if (collision == false) {
		gravity = true;
	}
	return true;
}



bool ModuleEnemy::PostUpdate()
{


	//SDL_Rect rect = currentAnimation->GetCurrentFrame();
	/* rect.h = 1035;
	 rect.w = 1194;
	 rect.x = 42;
	 rect.y = 55;*/
	//app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, 0, 0, 0);




	return true;
}

void ModuleEnemy::OnCollision(Collider* c1, Collider* c2)
{

	if (c1->type == Collider::Type::ENEMY && c2->type == Collider::Type::GROUND) {

		// app->player->position.y = 3198;
		collision = true;
		gravity = false;

	}
	if (c1->type == Collider::Type::ENEMYR && c2->type == Collider::Type::GROUND) {

		// app->player->position.y = 3198;
		collision = true;
		gravity = false;

	}
	if (c1->type == Collider::Type::ENEMYL && c2->type == Collider::Type::GROUND) {

		// app->player->position.y = 3198;
		collision = true;
		gravity = false;

	}

}