#include "ModulePhysics.h"
#include"ModulePlayer.h"
#include"Corazones.h"
#include "App.h"
#include "Log.h"
#include "Render.h"

#include "Input.h"
#include "SDL/include/SDL_Scancode.h"

ModuleCollisions::ModuleCollisions(bool startEnabled) : Module()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[Collider::Type::PLAYER][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::DEAD][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::DEAD] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::WIN] = true;
	matrix[Collider::Type::WIN][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::LEFT][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::LEFT] = true;
	matrix[Collider::Type::RIGHT][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::RIGHT] = true;
	matrix[Collider::Type::UP][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::UP] = true;
	matrix[Collider::Type::ENEMY][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::ENEMY][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::ENEMYR][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::ENEMYR] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYR] = true;
	matrix[Collider::Type::ENEMYR][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::ENEMYL][Collider::Type::GROUND] = true;
	matrix[Collider::Type::GROUND][Collider::Type::ENEMYL] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYL] = true;
	matrix[Collider::Type::ENEMYL][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::CORAZON][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::CORAZON] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::COIN] = true;
	matrix[Collider::Type::COIN][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::COIN2] = true;
	matrix[Collider::Type::COIN2][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::CHECKPOINT2][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::CHECKPOINT2] = true;
	matrix[Collider::Type::TELEPORT][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::TELEPORT] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYV] = true;
	matrix[Collider::Type::ENEMYV][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYVR] = true;
	matrix[Collider::Type::ENEMYVR][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYVL] = true;
	matrix[Collider::Type::ENEMYVL][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::MATAR][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::ENEMY][Collider::Type::MATAR] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYUP] = true;
	matrix[Collider::Type::ENEMYUP][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::ENEMYDOWN][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMYDOWN] = true;
	matrix[Collider::Type::MATAR][Collider::Type::ENEMYV] = true;
	matrix[Collider::Type::ENEMYV][Collider::Type::MATAR] = true;
}

// Destructor
ModuleCollisions::~ModuleCollisions()
{

}

bool ModuleCollisions::PreUpdate() {
	for (uint i = 0; i < MAX_COLLIDERS; i++) {
		if (colliders[i] != nullptr && colliders[i]->pendingToDelete == true) {
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; i++) {
		if (colliders[i] == nullptr) {
			continue;
		}
		c1 = colliders[i];
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k) {
			if (colliders[k] == nullptr) {
				continue;
			}
			c2 = colliders[k];

			if (matrix[c1->type][c2->type] && c1->Intersects(c2->rect))
			{
				for (uint i = 0; i < MAX_LISTENERS; ++i)
				{
					if (c1->listeners[i] != nullptr)
					{
						c1->listeners[i]->OnCollision(c1, c2);
						
					}
					
				}
				for (uint i = 0; i < MAX_LISTENERS; ++i)
				{
					if (c2->listeners[i] != nullptr) {
						c2->listeners[i]->OnCollision(c2, c1);
						
					}
					
				}
			}
		}
	}
	return true;
}

bool ModuleCollisions::Update(float dt)
{
	
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN ) {
		if (debug == true) {
			debug = false;
		}
		else {
			debug = true;
		}
	}
	

	return true;
}

bool ModuleCollisions::PostUpdate()
{
	if (debug == true)
		DebugDraw();
	
	return true;
}

void ModuleCollisions::DebugDraw()
{
	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case Collider::Type::NONE: // white
			app->render->DrawRectangle(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case Collider::Type::PLAYER: // green
			app->render->DrawRectangle(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case Collider::Type::ENEMY: // green
			app->render->DrawRectangle(colliders[i]->rect, 34, 255, 234, alpha);
			break;
		case Collider::Type::ENEMYV: // green
			app->render->DrawRectangle(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case Collider::Type::ENEMYR: // green
			app->render->DrawRectangle(colliders[i]->rect, 34, 255, 134, alpha);
			break; 
		case Collider::Type::ENEMYL: // green
				app->render->DrawRectangle(colliders[i]->rect, 34, 255, 194, alpha);
				break;
		case Collider::Type::ENEMYVR: // green
			app->render->DrawRectangle(colliders[i]->rect, 34, 255, 134, alpha);
			break;
		case Collider::Type::ENEMYVL: // green
			app->render->DrawRectangle(colliders[i]->rect, 34, 255, 194, alpha);
			break;
		case Collider::Type::ENEMYUP: // green
			app->render->DrawRectangle(colliders[i]->rect, 34, 155, 234, alpha);
			break;
		case Collider::Type::ENEMYDOWN: // green
			app->render->DrawRectangle(colliders[i]->rect, 134, 255, 134, alpha);
			break;
		case Collider::Type::GROUND: // green
			app->render->DrawRectangle(colliders[i]->rect, 0, 300, 10, alpha);
			break;

		case Collider::Type::DEAD: // green
			app->render->DrawRectangle(colliders[i]->rect, 80, 5, 150, alpha);
			break;
		case Collider::Type::WIN: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::RIGHT: // green
			app->render->DrawRectangle(colliders[i]->rect, 65, 2, 0, alpha);
			break;
		case Collider::Type::LEFT: // green
			app->render->DrawRectangle(colliders[i]->rect, 65, 2, 0, alpha);
			break;
		case Collider::Type::UP: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break; 
		case Collider::Type::CORAZON: // green
				app->render->DrawRectangle(colliders[i]->rect, 80, 25, 150, alpha);
				break;
		case Collider::Type::CHECKPOINT: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::CHECKPOINT2: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::COIN: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::COIN2: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::TELEPORT://green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
			break;
		case Collider::Type::MATAR: 
			app->render->DrawRectangle(colliders[i]->rect, 255, 50, 255, alpha);
			break;
		}
	}
	
}



// Called before quitting
bool ModuleCollisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* ModuleCollisions::AddCollider(SDL_Rect rect, Collider::Type type, Module* listener)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, listener);
			break;
		}
	}

	return ret;
}
