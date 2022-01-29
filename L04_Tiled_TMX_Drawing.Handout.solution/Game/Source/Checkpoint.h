#ifndef __CHECKPOINT_H__
#define __CHECKPOINT_H__

#include "Module.h"
#include "List.h"
#include "Scene.h"
#include "Animation.h"
#include "Point.h"
#include "Map.h"
#include "Render.h"

struct SDL_Texture;
//struct Collider;



class Checkpoint : public Module
{
public:
	Checkpoint();
	~Checkpoint();

	bool Start() override;


	// Processes new input and handles player movement
	bool Update(float dt);
	// Performs the render call of the player sprite
	bool PostUpdate();

public:
	List<SDL_Texture*> player;
	// Position of the player in the map
	iPoint position;
	iPoint Cposition;
	iPoint Cposition2;
	SDL_Texture* texture = nullptr;
	// The speed in which we move the player (pixels per frame)
	uint corazonFx = 0;
	bool jump = false;
	// The player spritesheet


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
	SDL_Texture* textureC = nullptr;
	SDL_Texture* textureC2 = nullptr;
	bool God = false;
	bool vida = true;
	bool win = false;
	Collider* checkpoint1 = nullptr;
	Collider* checkpoint2 = nullptr;
	bool musica = false;
	uint steps = 0000;
	uint limit = 00;
	uint stage = 00;
	bool corazon;
	bool print = false;
	bool print2 = false;
	int scoreFont = -1;
	char scoreText[10] = { "\0" };
	bool check = false;
	int limitFont = -1;
	char limitText[10] = { "\0" };
	int cont = 0;
	int stageFont = -1;
	char stageText[10] = { "\0" };

	bool canMove = true;
	int currentScene;
	uint numBox = 0;
	bool gravity = true;
	void OnCollision(Collider* c1, Collider* C2)override;
	bool playerMovement = true;
	bool collision = false;
	bool collision2 = false;
	bool dead;
	bool Win;
	bool Right = true;
	bool Left = true;
	bool godmode = false;
	bool playingM = false;
	//Sets the camera over the dot
	void set_camera();
};

#endif //!__MODULE_PLAYER_H__