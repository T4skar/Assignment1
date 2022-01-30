#include "GuiManager.h"
#include "App.h"
#include "Textures.h"

#include "GuiButton.h"
#include "Audio.h"
#include "Scene.h"
#include"ModulePlayer.h"

GuiManager::GuiManager() :Module()
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{

	textureButton = app->tex->Load("Assets/Sprite/UI.png");
	return true;
}

bool GuiManager::CleanUp()
{
	app->tex->UnLoad(textureButton);
	return true;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, SDL_Rect rect, const char* text)
{
	// L14: TODO1: Create a GUI control and add it to the list of controls

	GuiControl* control = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		control = new GuiButton(id, rect, text, clickedFx, focusedFx, textureButton);
		break;
	//case GuiControlType::CHECKBOX:
	//	control = new GuiCheckBox(id, rect, text, clickedFx, focusedFx, textureButton);
	//	break;
	//case GuiControlType::SLIDER:
	//	control = new GuiSlider(id, rect, text, clickedFx, focusedFx, textureButton, textureSlider);
	//	break;
	//

	default:
		break;
	}

	//Set the observer

	//control->SetObserver(observer);
	//control->SetTexture(texture);

	// Created GuiControls are added to the list of controls
	if (control != nullptr)
	{
		controls.add(control);
	}
	return control;
}

bool GuiManager::PostUpdate()
{

	if (app->scene->menu == true )
	{
		//SDL_Rect rect = pausedAnimationIn.GetCurrentFrame();
		/*app->render->DrawTexture(textureSlider, app->player->position.x, app->player->position.y - 200, &rect)
		if (rect.x != 73)
		{
			pausedAnimationIn.Update();
		}*/
	}

	ListItem<GuiControl*>* auxiliar;
	auxiliar = controls.start;
	while (auxiliar != nullptr)
	{
		auxiliar->data->Draw();
		auxiliar = auxiliar->next;
	}
	return true;
}

void GuiManager::DestroyGuiControl(GuiControl* entity)
{
	int idex = controls.find(entity);

	ListItem<GuiControl*>* auxiliar;
	auxiliar = controls.start;
	int cont = 0;
	while (auxiliar != nullptr)
	{
		if (cont == idex) {
			delete auxiliar->data;
			controls.del(auxiliar);
			auxiliar = nullptr;
			break;
		}
		cont++;
		auxiliar = auxiliar->next;
	}
}

bool GuiManager::PreUpdate()
{
	ListItem<GuiControl*>* auxiliar;
	auxiliar = controls.start;
	while (auxiliar != nullptr)
	{
		if (auxiliar->data->pendingToDelete == true)
		{
			delete auxiliar->data;
			controls.del(auxiliar);
		}
		auxiliar = auxiliar->next;
	}
	return true;
}

bool GuiManager::Update(float dt)
{	
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	ListItem<GuiControl*>* auxiliar;
	auxiliar = controls.start;
	while (auxiliar != nullptr)
	{
		auxiliar->data->Update(dt);
		auxiliar = auxiliar->next;
	}

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}
	
	return true;
}



void GuiManager::DestroyAllGuiControl()
{
	ListItem<GuiControl*>* auxiliar;
	auxiliar = controls.start;
	while (auxiliar != nullptr)
	{
		auxiliar->data->pendingToDelete = true;
		auxiliar = auxiliar->next;
	}

}

//
//bool GuiManager::UpdateAll(float dt, bool doLogic) {
//
//	if (doLogic) {
//
//		ListItem<GuiControl*>* control = controls.start;
//
//		while (control != nullptr)
//		{
//			control->data->Update(dt);
//			control = control->next;
//		}
//
//	}
//	return true; 
//
//}
//
//bool GuiManager::Draw() {
//
//	ListItem<GuiControl*>* control = controls.start;
//
//	while (control != nullptr)
//	{
//		control->data->Draw(app->render);
//		control = control->next;
//	}
//
//	return true;
//
//}

//bool GuiManager::CleanUp()
//{
//	ListItem<GuiControl*>* control = controls.start;
//
//	while (control != nullptr)
//	{
//		RELEASE(control);
//	}
//
//	return true;
//
//	return false;
//}



