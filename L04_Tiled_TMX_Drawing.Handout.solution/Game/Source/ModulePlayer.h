#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"

struct SDL_Texture;
struct Collider;



class ModulePlayer : public Module
{
public:
	ModulePlayer();
	~ModulePlayer();

	bool Start() override;


	// Processes new input and handles player movement
	bool Update();
	// Performs the render call of the player sprite
	bool PostUpdate() ;

public:

	// Position of the player in the map
	iPoint position;	
	int nPosX = 0;
	int nPosY = 0;
	// The speed in which we move the player (pixels per frame)
	int speed = 1;
	bool destroyed = false;
	// The player spritesheet
	SDL_Texture* texture = nullptr;
	SDL_Texture* table = nullptr;
	// The pointer to the current player animation
	Animation* currentAnimation = nullptr;

	// A set of animations
	Animation idleLeftAnim;
	Animation idleRightAnim;
	Animation idleUpAnim;
	Animation idleDownAnim;
	Animation upAnim;
	Animation downAnim;
	Animation leftAnim;
	Animation rightAnim;
	
	Animation boxupAnim;
	Animation boxdownAnim;
	Animation boxleftAnim;
	Animation boxrightAnim;

	Collider* collider = nullptr;

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


	bool playerMovement = true;
};

#endif //!__MODULE_PLAYER_H__