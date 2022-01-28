#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
//#include "GuiButton.h"
struct SDL_Texture;

class Scene : public Module
{
public:

	enum GameplayState
	{
		LOGO_SCREEN,
		TITLE_SCREEN,
		TITLE_MENU,
		PLAYING,
		PAUSE,
		GAME_OVER_SCREEN,
		CREDITS_SCREEN
	};



	Scene(bool startEnabled);

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);
	
	// Called before all Updates
	bool PostUpdate();
	bool dWin = false;
	bool dLose = false;
	SDL_Texture* lose = nullptr;
	SDL_Texture* Win = nullptr;
	

	//SDL_Texture* corazon = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_Texture* texture2 = nullptr;
	SDL_Texture* texture3 = nullptr;

	// Called before quitting

	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

	void LoadLevel(SString name, bool loadEntities = true);

	void ChangeGameplayState(GameplayState newState);
	void FadeToNewState(GameplayState newState);

	// Define multiple Gui Event methods
	//virtual bool OnGuiMouseClickEvent(GuiControl* control);

public:
	GameplayState gameplayState = LOGO_SCREEN;
	SDL_Texture* fondo = nullptr;
	bool playMusic = false;

	uint selectFx = 0;
	
	uint nextFx = 0;
	uint Mdead = 0;
	uint level1 = 0;
	bool playlev1=false;
	uint god = 0;

	void StartChTpColliders();
	void Pathfinding();
	//Teleports and Checkpoints
	void Checkpoints();
	bool Point = false;
	bool CheckU = false;
	bool Point2 = false;
	bool CheckU2 = false;
	int actualScene = 1;
	SDL_Texture* CheckpointText = nullptr;
	Collider* CheckP = nullptr;
	Collider* CheckP2 = nullptr;
	SDL_Texture* NameCheckText = nullptr;
	SDL_Texture* NameCheckText2 = nullptr;

	void Teleports();
	bool ActiveTeleport = false;
	int tps = 1;
	bool ActiveTeleport2 = false;
	int tps2 = 1;
	SDL_Texture* TeleportText = nullptr;
	SDL_Texture* Teleport2Text = nullptr;
	Collider* Teleport = nullptr;
	bool fading = false;


private:
	SDL_Texture* img;
	//Declare a GUI Button and create it using the GuiManager
	/*GuiButton* btn1;
	GuiButton* btn2;*/
};

#endif // __SCENE_H__