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
	 bool PostUpdate();
	 bool PreUpdate();
	 void DestroyAllGuiControl();
	//bool UpdateAll(float dt,bool logic);

	//bool Draw();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, SDL_Rect rect, const char* text);
	void DestroyGuiControl(GuiControl* entity);
	//void AddGuiControl(GuiControl* entity);

public:
	SDL_Texture* textureButton;
	SDL_Texture* textureSlider;
	List<GuiControl*> controls;
	//Animation pausedAnimationIn;
	//Animation pausedAnimationOut;
	unsigned int focusedFx;
	unsigned int clickedFx;
	unsigned int menuEfect;
	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;
	bool outAnimation;

	SDL_Texture* texture;

};

#endif // __GUIMANAGER_H__
