#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"


struct SDL_Texture;
//struct Collider;



class ModulePlayer : public Module
{
public:
	ModulePlayer();
	~ModulePlayer();

	bool Start() override;


	// Processes new input and handles player movement
	bool Update(float dt);
	// Performs the render call of the player sprite
	bool PostUpdate() ;

public:
	List<SDL_Texture*> player;
	// Position of the player in the map
	iPoint position;	

	// The speed in which we move the player (pixels per frame)
	
	bool jump = false;
	// The player spritesheet
	SDL_Texture* texture = nullptr;
	
	
	
	// The pointer to the current player animation
	Animation* currentAnimation;

	// A set of animations
	Animation idleLeftAnim;
	Animation idleRightAnim;
	Animation idleUpAnim;
	Animation idleDownAnim;
	Animation upAnim;
	Animation downAnim;
	Animation leftAnim;
	Animation rightAnim;
	
	bool God = false;
	bool vida = true;
	bool win = false;
	Collider* collider = nullptr;
	Collider* right = nullptr;
	Collider* left = nullptr;

	uint steps = 0000;
	uint limit = 00;
	uint stage = 00;

	int scoreFont = -1;
	char scoreText[10] = { "\0" };

	int limitFont = -1;
	char limitText[10] = { "\0" };

	int stageFont = -1;
	char stageText[10] = { "\0" };

	bool canMove = true;
	int currentScene;
	uint numBox=0;
	bool gravity = true;
	void OnCollision(Collider* c1, Collider* C2)override;
	bool playerMovement = true;
	bool collision;
	bool dead;
	bool Win;
};

#endif //!__MODULE_PLAYER_H__