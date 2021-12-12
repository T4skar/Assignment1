//#ifndef __CHECKPOINT_H__
//#define  __CHECKPOINT_H__
//
//#include "Module.h"
//#include "Animation.h"
//#include "Point.h"
//
//struct SDL_Texture;
//struct Collider;
//
//class Checkpoint : public Module
//{
//public:
//	// Constructor
//	Checkpoint();
//
//	// Destructor
//	~Checkpoint();
//
//	bool Start();
//
//	// Called at the middle of the application loop
//	// Processes new input and handles player movement
//	bool Update(float dt);
//
//	// Called at the end of the application loop
//	// Performs the render call of the player sprite
//	bool PostUpdate();
//
//
//public:
//	// Position of the player in the map
//	iPoint PChpoint;
//
//	float speed = 0.8f;
//	bool checkpoint = false;
//	bool Debug = false;
//	bool saved = false;
//	bool BlueFlag = false;
//	SDL_Texture* Chpoint=nullptr;
//	SDL_Rect Chpointrect;
//
//	Animation* currentAnimation;
//	Animation RFlag;
//	Animation BFlag;
//
//	// The player's collider
//	Collider* Chpointcoll = nullptr;
//
//	// A flag to detect when the player has been destroyed
//
//};
//
//#endif // __AUTOSAVE_H__