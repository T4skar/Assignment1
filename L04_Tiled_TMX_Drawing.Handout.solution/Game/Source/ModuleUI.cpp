#include "App.h"
#include "Textures.h"
#include "Window.h"
#include "Textures.h"
#include "Render.h"
#include "ModuleUI.h"
#include "ModulePlayer.h"
#include "Scene.h"
#include "Input.h"
#include "Collider.h"
#include "Audio.h"
//#include "Entities.h"
#include "GuiManager.h"

//#include "Optick/include/optick.h"

ModuleUI::ModuleUI() : Module()
{
	name.Create("ui");
}

ModuleUI::~ModuleUI()
{}

// Load assets
bool ModuleUI::Awake(pugi::xml_node& config)
{
	bool ret = true;

	pugi::xml_node uiPathN = config.child("ui");

	fontPath = uiPathN.attribute("fontPath").as_string();

	teleportMapPath1 = uiPathN.attribute("teleportMapPath1").as_string();
	teleportMapPath2 = uiPathN.attribute("teleportMapPath2").as_string();

	teleportArrowPath = uiPathN.attribute("teleportArrow").as_string();

	teleportCrossPath = uiPathN.attribute("teleportCross").as_string();

	livesTexturePath = uiPathN.attribute("livesTexturePath").as_string();

	optionsMenuPath = uiPathN.attribute("optionsMenu").as_string();
	settingsMenuPath = uiPathN.attribute("settingsMenu").as_string();
	saveMenuPath = uiPathN.attribute("saveMenu").as_string();
	saveDiskPath = uiPathN.attribute("saveDisk").as_string();

	menuArrowPath = uiPathN.attribute("menuArrow").as_string();


	score = 0;
	highScore = 0;

	currentLevel = 1;

	return ret;
}

bool ModuleUI::Start()
{
	bool ret = true;

	char lookupTable[] = { "0123456789.,\"!'-�ABCDEFGHIJKLMNOPQRSTUVWXYZ.:   " };

	font = Load(fontPath, lookupTable, 3);

	box = SDL_Rect({ 0, 0, app->render->camera.w, 30 });
	boxGodMode = SDL_Rect({ 0, 480, 38, 30 });
	boxShowColl = SDL_Rect({ 38, 480, 38, 30 });

	drawTeleportText = false;

	teleportMapLevel1 = app->tex->Load(teleportMapPath1);
	teleportMapLevel2 = app->tex->Load(teleportMapPath2);

	teleportArrowTex = app->tex->Load(teleportArrowPath);

	teleportCrossTex = app->tex->Load(teleportCrossPath);

	optionsMenuTex = app->tex->Load(optionsMenuPath);
	settingsMenuTex = app->tex->Load(settingsMenuPath);
	saveMenuTex = app->tex->Load(saveMenuPath);
	saveDiskTex = app->tex->Load(saveDiskPath);

	menuArrowTex = app->tex->Load(menuArrowPath);

	livesTexture = app->tex->Load(livesTexturePath);
	livesRect = SDL_Rect({ 0,0,12,10 });
	extraLivesRect = SDL_Rect({ 12,0,12,10 });

	destinationCheckpoint = 0;

	return ret;
}

// Update: draw background
bool ModuleUI::Update(float dt)
{
	if (uiToRender == 2)
		paused = true;
	else
		paused = false;

	if (!paused)
		timer += dt;

	if (app->saving)
	{
		app->saving = false;
		saveTimer = 0.0f;
	}

	if (saveTimer < maxSaveTimer)
		saveTimer += dt;
	else
		saveTimer = maxSaveTimer;

	switch (currentLevel)
	{
	case 1:
		arrow1.x = 92;
		arrow1.y = 190;

		arrow2.x = 211;
		arrow2.y = 106;

		arrow3.x = 325;
		arrow3.y = 106;

		arrow4.x = 375;
		arrow4.y = 205;

		crossPos1.x = 203;
		crossPos1.y = 116;

		crossPos2.x = 317;
		crossPos2.y = 116;

		checkpointCoordinates.clear();
		checkpointCoordinates.add(iPoint(176, 960));
		checkpointCoordinates.add(iPoint(1344, 512));
		checkpointCoordinates.add(iPoint(2384, 512));
		checkpointCoordinates.add(iPoint(2816, 1040));

		renderedMap = teleportMapLevel1;

		break;

	case 2:
		arrow1.x = 92;
		arrow1.y = 165;

		arrow2.x = 224;
		arrow2.y = 114;

		arrow3.x = 252;
		arrow3.y = 188;

		arrow4.x = 375;
		arrow4.y = 149;

		crossPos1.x = 216;
		crossPos1.y = 124;

		crossPos2.x = 245;
		crossPos2.y = 197;

		checkpointCoordinates.clear();
		checkpointCoordinates.add(iPoint(192, 816));
		checkpointCoordinates.add(iPoint(1456, 576));
		checkpointCoordinates.add(iPoint(1712, 944));
		checkpointCoordinates.add(iPoint(2784, 736));

		renderedMap = teleportMapLevel2;

		break;

	default:
		break;
	}



	switch (uiToRender)
	{
	case 1:
		if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING) uiToRender = 0;

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		{
			destinationCheckpoint++;
			if (destinationCheckpoint > 3)
			{
				destinationCheckpoint = 3;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		{
			destinationCheckpoint--;
			if (destinationCheckpoint < 0)
			{
				destinationCheckpoint = 0;
			}
		}

		switch (destinationCheckpoint)
		{
		case 0:
			renderedArrowPos = arrow1;
			break;

		case 1:
			renderedArrowPos = arrow2;
			break;

		case 2:
			renderedArrowPos = arrow3;
			break;
		case 3:
			renderedArrowPos = arrow4;
			break;

		default:
			break;
		}

		//if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		//{
		//	app->audio->PlayFx(app->entities->GetPlayer()->gameStartFx, 0);

		//	switch (destinationCheckpoint)
		//	{
		//	case 0:
		//		app->entities->GetPlayer()->position = fPoint(checkpointCoordinates[0].x, checkpointCoordinates[0].y);
		//		app->entities->GetPlayer()->collider->SetPos(app->entities->GetPlayer()->position.x, app->entities->GetPlayer()->position.y);
		//		uiToRender = 0;
		//		break;

		//	case 1:
		//		app->entities->GetPlayer()->position = fPoint(checkpointCoordinates[1].x, checkpointCoordinates[1].y);
		//		app->entities->GetPlayer()->collider->SetPos(app->entities->GetPlayer()->position.x, app->entities->GetPlayer()->position.y);
		//		uiToRender = 0;
		//		break;

		//	case 2:
		//		app->entities->GetPlayer()->position = fPoint(checkpointCoordinates[2].x, checkpointCoordinates[2].y);
		//		app->entities->GetPlayer()->collider->SetPos(app->entities->GetPlayer()->position.x, app->entities->GetPlayer()->position.y);
		//		uiToRender = 0;
		//		break;

		//	case 3:
		//		app->entities->GetPlayer()->position = fPoint(checkpointCoordinates[3].x, checkpointCoordinates[3].y);
		//		app->entities->GetPlayer()->collider->SetPos(app->entities->GetPlayer()->position.x, app->entities->GetPlayer()->position.y);
		//		uiToRender = 0;
		//		break;

		//	default:
		//		break;
		//	}
		//}
		break;

	case 2:
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING)
		{
			app->guiManager->DestroyAllGuiControls();
			uiToRender = 0;
		}
		break;

	case 3:
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING)
		{
			app->guiManager->DestroyAllGuiControls();
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 204, 69, 73, 35 }), 1);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 197, 105, 87, 35 }), 2);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 177, 141, 127, 35 }), 3);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 212, 177, 56, 35 }), 4);
			uiToRender = 2;
		}
		break;

	case 4:
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING)
		{
			app->guiManager->DestroyAllGuiControls();
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 204, 69, 73, 35 }), 1);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 197, 105, 87, 35 }), 2);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 177, 141, 127, 35 }), 3);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 212, 177, 56, 35 }), 4);
			uiToRender = 2;
		}
		break;

	default:
		if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING) uiToRender = 1;
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && app->scene->gameplayState == Scene::GameplayState::PLAYING)
		{
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 204, 69, 73, 35 }), 1);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 197, 105, 87, 35 }), 2);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 177, 141, 127, 35 }), 3);
			app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 212, 177, 56, 35 }), 4);
			uiToRender = 2;
		}
		break;
	}

	canDrawSecret = false;
	drawTeleportText = false;

	//toLoadBar = 24 * app->entities->GetPlayer()->cooldown / app->entities->GetPlayer()->maxCooldown;

	//boxCooldown = SDL_Rect({ (int)app->entities->GetPlayer()->position.x, (int)app->entities->GetPlayer()->position.y - 10, toLoadBar, 1 });
	//boxOuterCooldown = SDL_Rect({ (int)app->entities->GetPlayer()->position.x - 1, (int)app->entities->GetPlayer()->position.y - 11, 26, 3 });

	if (score > highScore)
		highScore = score;

	return true;
}

//bool ModuleUI::PostUpdate()
//{
//	OPTICK_EVENT("UIPostUpdate", Optick::Category::UI);
//
//	app->render->DrawRectangle(box, 33, 31, 48, 255, true, false);
//
//	if (app->entities->GetPlayer()->cooldown < app->entities->GetPlayer()->maxCooldown)
//	{
//		app->render->DrawRectangle(boxOuterCooldown, 33, 31, 48, 255, true, true);
//		app->render->DrawRectangle(boxCooldown, 71, 174, 72, 255, false, true);
//	}

	//int uiposx = 10;
	//BlitText(uiposx, 5, font, "LEVEL", false);
	//IntToString(shortNumberText, currentLevel, 2);
	//BlitText(uiposx + 55, 5, font, shortNumberText, false);

	//BlitText(uiposx + 90, 5, font, "HEALTH", false);

	//for (int i = 0; i < app->entities->GetPlayer()->health; i++)
	//{
	//	if (i < 3)
	//	{
	//		app->render->DrawTexture(livesTexture, uiposx + 150 + (i * 15), 3, &livesRect, 0, 0, 0, 0, false);
	//	}
	//	else if (i < 6)
	//	{
	//		app->render->DrawTexture(livesTexture, uiposx + 150 + (i * 15), 3, &extraLivesRect, 0, 0, 0, 0, false);
	//	}
	//}

	//if (app->entities->GetPlayer()->godMode)
	//{
	//	app->render->DrawRectangle(boxGodMode, 33, 31, 48, 255, true, false);
	//	BlitText(5, app->render->camera.h / 2 - 25, font, "G", false);
	//}

	//if (app->physics->showColliders)
	//{
	//	app->render->DrawRectangle(boxShowColl, 33, 31, 48, 255, true, false);
	//	BlitText(24, app->render->camera.h / 2 - 25, font, "C", false);
	//}

	//BlitText(uiposx + 320, 5, font, "SCORE", false);
	//IntToDynamicString(scoreText, score);
	//BlitText(uiposx + 375, 5, font, scoreText, false);

	//int seconds = (int)timer;
	//int minutes = seconds / 60;
	//seconds = seconds % 60;

	//app->render->DrawRectangle(SDL_Rect({ 0, app->render->camera.h - 30, app->render->camera.w, 30 }), 33, 31, 48, 255, true, false);

	//BlitText(10, app->render->camera.h / 2 - 10, font, "HIGHSCORE", false);

	//char highscoreText[11];
	//IntToDynamicString(highscoreText, highScore);
	//BlitText(100, app->render->camera.h / 2 - 10, font, highscoreText, false);

	//char minutesText[3];
	//IntToDynamicString(minutesText, minutes, 2);
	//char secondsText[3];
	//IntToDynamicString(secondsText, seconds, 2);
	//BlitText(app->render->camera.w / 2 - 99, app->render->camera.h / 2 - 10, font, "TIMER", false);
	//BlitText(app->render->camera.w / 2 - 45, app->render->camera.h / 2 - 10, font, minutesText, false);
	//BlitText(app->render->camera.w / 2 - 29, app->render->camera.h / 2 - 11, font, ":", false);
	//BlitText(app->render->camera.w / 2 - 21, app->render->camera.h / 2 - 10, font, secondsText, false);

	/*if (canDrawSecret)
	{
		BlitText(app->entities->GetPlayer()->position.x - 70, app->entities->GetPlayer()->position.y + 30, font, "ONLY A GOD CAN REACH", true);
		BlitText(app->entities->GetPlayer()->position.x - 70, app->entities->GetPlayer()->position.y + 40, font, "MIDAS' HALL", true);
	}*/

	//switch (uiToRender)
//	{
//	case 1:
//		app->render->DrawTexture(renderedMap, 0, 0, NULL, 0, 0, 0, 0, false);
//		app->render->DrawTexture(teleportArrowTex, renderedArrowPos.x, renderedArrowPos.y, NULL, 0, 0, 0, 0, false);
//		break;
//
//	case 2:
//		app->render->DrawTexture(optionsMenuTex, 0, 0, NULL, 0, 0, 0, 0, false);
//		break;
//
//	case 3:
//		app->render->DrawTexture(settingsMenuTex, 0, 0, NULL, 0, 0, 0, 0, false);
//		break;
//
//	case 4:
//		app->render->DrawTexture(saveMenuTex, 0, 0, NULL, 0, 0, 0, 0, false);
//		break;
//
//	default:
//		break;
//	}
//
//	if (saveTimer < maxSaveTimer)
//		app->render->DrawTexture(saveDiskTex, 10, 25, NULL, 0, 0, 0, 0, false);
//
//	return true;
//}

bool ModuleUI::CleanUp()
{

	bool ret = true;

	UnLoad(font);

	return ret;
}


// Load new texture from file path
int ModuleUI::Load(const char* texturePath, const char* characters, uint rows)
{
	int id = -1;

	if (texturePath == nullptr || characters == nullptr || rows == 0)
	{
		return id;
	}

	SDL_Texture* tex = app->tex->Load(texturePath);

	if (tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		return id;
	}

	id = 0;
	for (; id < MAX_FONTS; ++id)
		if (fonts[id].texture == nullptr)
			break;

	if (id == MAX_FONTS)
	{
		return id;
	}

	Font& font = fonts[id];

	font.texture = tex;
	font.rows = rows;

	// TODO 1: Finish storing font data

	// totalLength ---	length of the lookup table
	// table ---------  All characters displayed in the same order as the texture
	// columns -------  Amount of chars per row of the texture
	// char_w --------	Width of each character
	// char_h --------	Height of each character

	strcpy_s(fonts[id].table, MAX_FONT_CHARS, characters);
	font.totalLength = strlen(characters);
	font.columns = fonts[id].totalLength / rows;

	uint texW, texH;
	app->tex->GetSize(tex, texW, texH);
	font.charW = texW / font.columns;
	font.charH = texH / font.rows;


	k++;

	return id;
}

void ModuleUI::UnLoad(int fontId)
{
	if (fontId >= 0 && fontId < MAX_FONTS && fonts[fontId].texture != nullptr)
	{
		app->tex->UnLoad(fonts[fontId].texture);
		fonts[fontId].texture = nullptr;
	}
	k--;
}

void ModuleUI::BlitText(int x, int y, int fontId, const char* text, bool useCamera) const
{
	if (text == nullptr || fontId < 0 || fontId >= MAX_FONTS || fonts[fontId].texture == nullptr)
	{
		return;
	}

	const Font* font = &fonts[fontId];
	SDL_Rect spriteRect;
	uint len = strlen(text);

	spriteRect.w = font->charW;
	spriteRect.h = font->charH;

	for (uint i = 0; i < len; ++i)
	{
		uint charIndex = 0;

		// Find the location of the current character in the lookup table
		for (uint j = 0; j < font->totalLength; ++j)
		{
			if (font->table[j] == text[i])
			{
				charIndex = j;
				break;
			}
		}

		// Retrieve the position of the current character in the sprite
		spriteRect.x = spriteRect.w * (charIndex % font->columns);
		spriteRect.y = spriteRect.h * (charIndex / font->columns);

		app->render->DrawTexture(font->texture, x, y, &spriteRect, 1.0f, 0.0f, INT_MAX, INT_MAX, useCamera);

		// Advance the position where we blit the next character
		x += spriteRect.w;
	}
}

void ModuleUI::IntToDynamicString(char* buffer, int k, int len)
{

	for (int i = 0; i < len; i++)
	{
		buffer[i] = '0';
	}

	buffer[len] = 0;

	int i = len - 1;
	while (k != 0)
	{
		if (i < 0) break;
		buffer[i--] += k % 10;
		k /= 10;
	}
}

void ModuleUI::IntToString(char* buffer, int k, int length)
{

	for (int i = 0; i < length; i++)
	{
		buffer[i] = '0';
	}

	buffer[length] = 0;

	int i = length - 1;
	while (k != 0)
	{
		if (i < 0) break;
		buffer[i--] += k % 10;
		k /= 10;
	}
}