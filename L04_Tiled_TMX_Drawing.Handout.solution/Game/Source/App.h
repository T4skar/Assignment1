#ifndef __APP_H__
#define __APP_H__
//
#include "Module.h"
#include "ModuleEnemyVolador.h"
#include "ModuleEnemy.h"
#include "List.h"
#include"PerfTimer.h"
#include"Timer.h"
#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class ModulePlayer;
class Map;
class ModuleFadeToBlack;
class ModuleRender;
class ModuleCollisions;
class SceneLogo;
class ModuleCamera;
class ModuleEnemy;
class SceneTitle;
//class Pathfinding;
class ModuleEnemyVolador;
class Corazones;
class Checkpoint;
class ModuleCoin;
class EntityManeger;

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};


class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	// L02: DONE 1: Create methods to request Load / Save
	void LoadGameRequest();
	void SaveGameRequest() const;

private:

	// Load config file
	// NOTE: It receives config document
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGame();
	bool SaveGame() const;

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	Scene* scene;
	ModulePlayer* player;
	Map* map;
	ModuleFadeToBlack* fade;
	ModuleCollisions* physics;
	SceneLogo* LOGO;
	ModuleEnemy* enemy;
	SceneTitle* Title;
	ModuleEnemyVolador* enemyvol;
	//Pathfinding* pathfinding;
	Corazones* corazon;
	Checkpoint* checkp;
	ModuleCoin* coin;
	EntityManeger* entities;




	

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float dt = 0.0f;

	uint32 maxFrameRate = 0;
	
private:
	PerfTimer* ptimer;
	PerfTimer* frameDuration;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module*> modules;



	uint frames;
	

	mutable bool saveGameRequested;
	bool loadGameRequested;
};

extern App* app;

#endif	// __APP_H__