#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"


class Collider;


enum class EntityType
{
	PLAYER,
	ENEMY,
	COIN,
	HEART,
	UNKNOWN
};

enum EnemyType
{
	NO_TYPE,
	SUELO,
	VOLADOR
};

class Entity
{
public:
	// Constructor
	Entity(EntityType type) : type(type), active(true) {}
	{}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Blit
	virtual bool Draw()
	{
		return true;
	}

	// Collision response
	virtual void OnCollision(Collider* c1, Collider* c2)
	{}


public:
	EntityType type;
	SDL_Rect entityRect;
	//Collider* collider;
	//Physics physics;
	iPoint nextPos;
	uint destroyedFx;

	bool pendingToDelete = false;

	iPoint spawnPos;


	//Animation* currentAnim = nullptr;
	bool invert = false;

	bool isDead = false;

	EnemyType eType;

};

#endif // __ENTITY_H__