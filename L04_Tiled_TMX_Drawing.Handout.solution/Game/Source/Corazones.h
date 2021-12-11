#ifndef __MODULE_CORAZONES_H__
#define __MODULE_CORAZONES_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"
#include "Map.h"
#include "Render.h"

struct SDL_Texture;
//struct Collider;



class Corazones : public Module
{
public:
	Corazones();
	~Corazones();

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
	SDL_Texture* texture2 = nullptr;
	SDL_Texture* texture3 = nullptr;
	
	//Save i load
	bool loadState(pugi::xml_node&);
	bool saveState(pugi::xml_node&) const;
	
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
	
	bool God = false;
	bool vida = true;
	bool win = false;
	Collider* cora = nullptr;


	uint steps = 0000;
	uint limit = 00;
	uint stage = 00;
	bool corazon;
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
	bool Right=true;
	bool Left=true;
	bool godmode = false;
	bool playingM = false;
	//Sets the camera over the dot
	void set_camera();
};

#endif //!__MODULE_PLAYER_H__