#include "GuiManager.h"
#include "App.h"
#include "Textures.h"
#include "GuiSlider.h"
#include "GuiButton.h"
#include "Audio.h"

GuiManager::GuiManager() :Module()
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	return true;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int x, int y, SDL_Rect bounds, int id)
{
	GuiControl* control = nullptr;

	switch (type)
	{
	case GuiControlType::BUTTON:
		control = new GuiButton(id, bounds, arrowMenuTex);
		control->SetObserver(app->scene);
		break;
	//case GuiControlType::CHECKBOX:
	//	control = new GuiCheckBox(id, bounds, checkBoxTex);
	//	control->SetObserver(app->scene);
	//	break;
	case GuiControlType::SLIDER:
		control = new GuiSlider(id, bounds, sliderTex);
		control->SetObserver(app->scene);
		break;

	default: break;
	}

	// Created entities are added to the list
	if (control != nullptr) controls.add(control);

	return control;
}

bool GuiManager::Update(float dt)
{
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	return true;
}

bool GuiManager::UpdateAll(float dt, bool doLogic) {

	if (doLogic) {

		ListItem<GuiControl*>* control = controls.start;

		while (control != nullptr)
		{
			control->data->Update(dt);
			control = control->next;
		}

	}
	return true;

}

bool GuiManager::Draw() {

	ListItem<GuiControl*>* control = controls.start;

	while (control != nullptr)
	{
		control->data->Draw(app->render);
		control = control->next;
	}

	return true;

}

bool GuiManager::CleanUp()
{
	ListItem<GuiControl*>* control = controls.start;

	while (control != nullptr)
	{
		RELEASE(control);
	}

	return true;

	return false;
}

void GuiManager::DestroyAllGuiControls()
{
	int u = controls.count();

	for (int i = 0; i < u; i++)
	{
		delete controls.At(0)->data;
		controls.del(controls.At(0));
	}
}

void GuiManager::DestroyGuiControl(GuiControl* entity)
{
	int i = controls.find(entity);
	ListItem<GuiControl*>* c = controls.At(i);
	controls.del(c);
}