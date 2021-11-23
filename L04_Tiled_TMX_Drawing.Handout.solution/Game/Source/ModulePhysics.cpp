#include "ModulePhysics.h"
#include"ModulePlayer.h"
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
					app->player->collision = false;
				}
				for (uint i = 0; i < MAX_LISTENERS; ++i)
				{
					if (c2->listeners[i] != nullptr) {
						c2->listeners[i]->OnCollision(c2, c1);
					}
					app->player->collision = false;
				}
			}
		}
	}
	return true;
}

bool ModuleCollisions::Update(float dt)
{
	
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN ) {
		if (debug == true) {
			debug = false;
		}
		else {
			debug = true;
		}
	}
	
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		
		app->player->God = !app->player->God;

		app->player->gravity = 0.0f;
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			app->player->position.y -= 1;
			app->render->camera.y = app->player->position.y;

		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			app->player->position.y = app->player->position.y;
			app->render->camera.y = app->player->position.y;

		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			app->player->position.x -= 1;
			app->render->camera.x = app->player->position.x;


		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			app->player->position.x += 1;
			app->render->camera.x = app->player->position.x;

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
		case Collider::Type::GROUND: // green
			app->render->DrawRectangle(colliders[i]->rect, 0, 300, 10, alpha);
			break;

		case Collider::Type::DEAD: // green
			app->render->DrawRectangle(colliders[i]->rect, 80, 5, 150, alpha);
			break;
		case Collider::Type::WIN: // green
			app->render->DrawRectangle(colliders[i]->rect, 3, 25, 150, alpha);
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
