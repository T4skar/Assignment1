//#ifndef __MODULE_PHYSICS_H__
//#define __MODULE_PHYSICS_H__
//
//#include "Module.h"
//#include "List.h"
//#include "Scene.h"
//#include "Animation.h"
//#include "Point.h"


#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)
//
//struct SDL_Texture;
//struct Collider;


//
//
//class ModulePlayer : public Module
//{
//public:
//	
//
//	bool Start() override;
//
//
//	// Processes new input and handles player movement
//	bool Update(float dt);
//	// Performs the render call of the player sprite
//	bool PostUpdate() ;
//
//public:
//	List<SDL_Texture*> player;
//	// Position of the player in the map
//	iPoint position;	
//
//	// The speed in which we move the player (pixels per frame)
//	
//	
//	// The player spritesheet
//	SDL_Texture* texture = nullptr;
//	
//	// The pointer to the current player animation
//	Animation* currentAnimation;
//
//	// A set of animations
//	Animation idleLeftAnim;
//	Animation idleRightAnim;
//	Animation idleUpAnim;
//	Animation idleDownAnim;
//	Animation upAnim;
//	Animation downAnim;
//	Animation leftAnim;
//	Animation rightAnim;
//	
//	
//
//	Collider* collider = nullptr;
//
//	uint steps = 0000;
//	uint limit = 00;
//	uint stage = 00;
//
//	int scoreFont = -1;
//	char scoreText[10] = { "\0" };
//
//	int limitFont = -1;
//	char limitText[10] = { "\0" };
//
//	int stageFont = -1;
//	char stageText[10] = { "\0" };
//
//	bool canMove = true;
//	int currentScene;
//	uint numBox=0;
//
//
//	bool playerMovement = true;
//};

//#endif //!__MODULE_PLAYER_H__