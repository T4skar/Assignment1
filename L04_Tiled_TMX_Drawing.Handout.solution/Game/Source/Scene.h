#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "GuiButton.h"
#include"Timer.h"

struct SDL_Texture;

class Scene : public Module
{
public:

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
	SDL_Texture* Moneda = nullptr;

	//SDL_Texture* corazon = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_Texture* texture2 = nullptr;
	SDL_Texture* texture3 = nullptr;
	SDL_Texture* Credits = nullptr;
	int credity = 0;
	// Called before quitting
	bool CleanUp();
	SDL_Texture* fondo = nullptr;
	bool playMusic = false;
	SDL_Texture* font = nullptr;
	uint selectFx = 0;
	uint SelectedFx = 0;
	//uint SelectedFx2 = 0;
	uint nextFx = 0;
	uint Mdead = 0;
	uint level1 = 0;
	bool playlev1=false;
	uint god = 0;
	char timer[6] = { "\0" };
	void StartChTpColliders();
	//void Pathfinding();
	//Teleports and Checkpoints
	void Checkpoints();
	bool Point = false;
	bool CheckU = false;
	bool Point2 = false;
	bool CheckU2 = false;
	bool Menu = false;
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
	bool play = false;
	bool OnGuiMouseClickEvent(GuiControl* control);
	bool credits = false;
	//Timer timer;

	bool exit = false;
	bool settings = false;
	bool musicon = false;
	bool musiconfx = false;
	bool fullscreen = false;
	bool vsyncon = false;
	bool returnto = false;
	bool musicasi = true;
	bool continuar = false;

private:
	SDL_Texture* img;

	GuiButton* btn1;
	GuiButton* btn2; 
	GuiButton* btn3;
	GuiButton* btn4;
	GuiButton* btn5;
	GuiButton* btn6;
	GuiButton* btn7;
	GuiButton* btn8;
	GuiButton* btn9;
	GuiButton* btn10;
	

};

#endif // __SCENE_H__