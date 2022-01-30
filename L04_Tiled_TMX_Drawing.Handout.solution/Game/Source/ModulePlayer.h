#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"
#include "Map.h"
#include "Render.h"

struct SDL_Texture;
//struct Collider;


class position
{
public:
	~position() {}
	int x, y;
};
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
	//iPoint position;	

	position position;

	// The speed in which we move the player (pixels per frame)
	
	bool jump = false;
	// The player spritesheet
	SDL_Texture* texture = nullptr;
	
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
	//bool vida = true;
	bool win = false;
	Collider* collider = nullptr;
	Collider* right = nullptr;
	Collider* left = nullptr;
	Collider* up = nullptr;
	Collider* matar = nullptr;
	bool musica = false;
	uint steps = 0000;
	uint limit = 00;
	uint stage = 00;
	uint corazonFx = 0;
	int scoreFont = -1;
	char scoreText[10] = { "\0" };

	int limitFont = -1;
	char limitText[10] = { "\0" };
	bool salto = false;
	/*float speed = 1.0f;*/

	int vidas = 4;

	//Checkpoint points and check state
	bool CheckActive = false;
	bool CheckActive2 = false;
	uint GetCheckpoint = 0;

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
	uint Mdead = 0;
	bool Mlose=true;
	//Sets the camera over the dot
	void set_camera();
};

#endif //!__MODULE_PLAYER_H__