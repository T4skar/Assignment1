#include "SceneCredits.h" 
#include "ModulePlayer.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
//#include "ModuleFonts.h" 
#include "Input.h"

#include "SDL/include/SDL_Scancode.h"


SceneLogo::SceneLogo(bool startEnabled) : Module()
{
	name.Create("scene logo");
}

SceneLogo::~SceneLogo()
{ 

}

bool SceneLogo::Start()
{
	LOG("Loading background assets");

	bool ret = true;

	logo = app->tex->Load("Assets/textures/logotroll.png");

	

	return ret;
}

bool SceneLogo::Update()
{
	
	
	app->render->DrawTexture(logo, 0, 0);
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN )
	{
		/*app->player->active = true;
		app->scene->active = true;
		app->render->active = false;
		active = false;*/
		
	}

	return true;
}

bool SceneLogo::PostUpdate()
{
	

	return true;
}
bool SceneLogo::CleanUp() {
	
	return true;
}