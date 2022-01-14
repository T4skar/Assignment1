#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"

#include "List.h"

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager();

	// Destructor
	virtual ~GuiManager();

	// Called before the first frame
	bool Start();

	bool Update(float dt);

	bool UpdateAll(float dt, bool logic);

	bool Draw();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int x, int y, SDL_Rect bounds, int id);
	void DestroyGuiControl(GuiControl* entity);
	void DestroyAllGuiControls();
	void AddGuiControl(GuiControl* entity);

public:

	List<GuiControl*> controls;

	void DestroyAllGuiControls();
	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	bool showDebug = false;

	const char* arrowMenuPath;
	const char* checkBoxPath;
	const char* sliderPath;

	SDL_Texture* arrowMenuTex;
	SDL_Texture* checkBoxTex;
	SDL_Texture* sliderTex;

	const char* hoverButtonFxPath;
	const char* pressButtonFxPath;
	const char* checkboxFxPath;

	uint hoverButtonFx = 0;
	uint pressButtonFx = 0;
	uint checkboxFx;

	int lastId = 0;

	int fxVolume = 100;
	int musicVolume = 100;

	bool fullscreenChecked = false;
	bool vSyncChecked = false;


	SDL_Texture* texture;

};

#endif // __GUIMANAGER_H__