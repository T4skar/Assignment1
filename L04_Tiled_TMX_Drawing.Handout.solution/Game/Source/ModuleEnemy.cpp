#include "ModuleEnemy.h"
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

ModuleEnemy::ModuleEnemy() : Module()
{

	// Aqui van las animaciones del player
	idleRightAnim.PushBack({ 47,103,92,75 });
	idleRightAnim.PushBack({ 171,103,92,75 });
	idleRightAnim.loop = true;
	idleRightAnim.speed = 0.006f;

	rightAnim.PushBack({ 47,103,92,75 });
	rightAnim.PushBack({ 171,103,92,75 });
	rightAnim.PushBack({ 47,103,92,75 });
	rightAnim.PushBack({ 171,103,92,75 });
	rightAnim.loop = true;
	rightAnim.speed = 0.03f;

	dieAnim.PushBack({ 47,103,92,75 });
	dieAnim.PushBack({ 171,103,92,75 });
	dieAnim.PushBack({ 1086,103,92,75 });
	dieAnim.PushBack({ 1086,0,92,75 });
	dieAnim.loop = true;
	dieAnim.speed = 0.03f;

	leftAnim.PushBack({ 47,103,92,75 });
	leftAnim.PushBack({ 171,103,92,75 });
	leftAnim.PushBack({ 47,103,92,75 });
	leftAnim.PushBack({ 171,103,92,75 });
	leftAnim.loop = true;
	leftAnim.speed = 0.03f;


}



ModuleEnemy::~ModuleEnemy()
{
}


bool ModuleEnemy::Start()
{
	LOG("Loading enemy textures");

	bool ret = true;

	texture = app->tex->Load("Assets/Sprites/Slime.png");

	currentAnimation = &idleRightAnim;

	position.x = 7000;
	position.y = 80;
	// X, Y, anchura, altura, 
	//collider = app->physics->AddCollider({ position.x, position.y, 115, 171 }, Collider::Type::ENEMY, this);

	colliderE = app->physics->AddCollider({ position.x, position.y, 92,75 }, Collider::Type::ENEMY, this);
	enemyRight = app->physics->AddCollider({ position.x, position.y, 500, 75 }, Collider::Type::ENEMYR, this);
	enemyLeft = app->physics->AddCollider({ position.x, position.y, 500, 75 }, Collider::Type::ENEMYL, this);
	//enemyLeft = app->physics->AddCollider({ position.x, position.y, -500, 80 }, Collider::Type::ENEMYL, this);
	return ret;
}

bool ModuleEnemy::Update(float dt)
{
	float speed = 1.0f * dt * 0.3;
	collision = false;

	colliderE->SetPos(position.x, position.y - 14);
	enemyRight->SetPos(position.x, position.y - 14);
	enemyLeft->SetPos(position.x - 500, position.y - 14);

	if (dead == true)
	{
		gravity = false;
		right = false;
		left = false;
		position.y = 11000;
		position.x = 11000;

		if (currentAnimation != &dieAnim) {
			dieAnim.Reset();
			currentAnimation = &dieAnim;
		}

	}
	else
	{
		collision = false;
	}
	
	
	if (gravity == false) {
		position.y += 0;
	}
	if (app->input->GetKey(SDL_SCANCODE_1 == KEY_DOWN)) {
		position.x = 5200;
			position.y = 80;
	
	}
	if (gravity == true) {
		position.y += 2 * speed * 0.60;
	}

	if (collision == false) {
		gravity = true;
	}

	if (dead == true)
	{
		collision = false;
	}

	if (left == true) {
		position.x -= speed;

		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}
	else if (right == true) {
		position.x += speed;

		if (currentAnimation != &rightAnim) {
			rightAnim.Reset();
			currentAnimation = &rightAnim;
		}
	}
	else {

		if (currentAnimation != &idleRightAnim) {
			idleRightAnim.Reset();
			currentAnimation = &idleRightAnim;
		}
	}
	currentAnimation->Update();
	dead = false;
	right = false;
	left = false;
	return true;



}



bool ModuleEnemy::PostUpdate()
{


	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, 0, 0, 0);




	return true;
}

void ModuleEnemy::OnCollision(Collider* c1, Collider* c2)
{

	if (c1->type == Collider::Type::ENEMY && c2->type == Collider::Type::GROUND) {


		collision = true;
		gravity = false;

	}

	if (c1->type == Collider::Type::ENEMY && c2->type == Collider::Type::PLAYER) {


		dead = true;

	}
	if (c1->type == Collider::Type::ENEMY && c2->type == Collider::Type::MATAR) {


		dead = true;

	}
	if (c1->type == Collider::Type::ENEMYL && c2->type == Collider::Type::PLAYER) {



		left = true;

	}
	if (c1->type == Collider::Type::ENEMYR && c2->type == Collider::Type::PLAYER) {



		right = true;

	}
}
bool ModuleEnemy::CleanUp()
{
	/*LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (enemy[i] != nullptr)
		{
			delete enemy[i];
			enemy[i] = nullptr;
		}
	}*/

	return true;
}