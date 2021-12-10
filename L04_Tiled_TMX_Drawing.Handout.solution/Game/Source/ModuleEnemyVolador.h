#ifndef __MODULE_ENEMYVOLADOR_H__
#define __MODULE_ENEMYVOLADOR_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"
#include "Map.h"
#include "Render.h"

struct SDL_Texture;




class ModuleEnemyVolador : public Module
{
public:
	ModuleEnemyVolador(bool startEnabled);
	~ModuleEnemyVolador();

	bool Start() override;


	
	bool Update(float dt);
	
	bool PostUpdate();

public:
	List<SDL_Texture*> enemy;
	// Position of the player in the map
	iPoint position;

	// The speed in which we move the player (pixels per frame)

	
	// The player spritesheet
	SDL_Texture* texture = nullptr;



	// The pointer to the current player animation
	Animation* currentAnimation;

	// A set of animations
	Animation idle;
	Animation idleRightAnim;
	Animation idleUpAnim;
	Animation idleDownAnim;
	Animation upAnim;
	Animation downAnim;
	Animation leftAnim;
	Animation rightAnim;
	Animation atackRightAnim;
	Animation atackLeftAnim;

	
	bool vida = true;
	bool win = false;
	Collider* colliderE = nullptr;
	Collider* enemyRight = nullptr;
	Collider* enemyLeft = nullptr;
	Collider* up = nullptr;

	uint steps = 0000;
	uint limit = 00;
	uint stage = 00;

	
	bool canMove = true;
	int currentScene;
	uint numBox = 0;
	bool gravity = true;
	void OnCollision(Collider* c1, Collider* C2)override;
	bool enemyMovement = true;
	bool collision;
	bool dead;
	//bool enemyRight = true;
	//bool enemyLeft = true;

	//Sets the camera over the dot
	
};

#endif //!__MODULE_PLAYER_H__