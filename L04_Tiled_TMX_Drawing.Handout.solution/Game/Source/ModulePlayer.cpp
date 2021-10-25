#include "ModulePlayer.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include "Defs.h"
#include "Log.h"


//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

ModulePlayer::ModulePlayer() 
{

	// Aqui van las animaciones del player
	idleLeftAnim.PushBack({ 1050,670,110,170 });
	idleUpAnim.PushBack({ 38,55,50,60 });
	idleRightAnim.PushBack({ 40,55,50,60 });
	idleDownAnim.PushBack({ 45,58,50,60 });
	rightAnim.loop = true;
	rightAnim.speed = 0.3f;

	/*leftAnim.PushBack({ 42,55,14,24 });
	leftAnim.PushBack({ 33,9,14,24 });
	leftAnim.PushBack({ 57,9,14,24 });
	leftAnim.PushBack({ 81,9,14,24 });
	leftAnim.PushBack({ 106,9,14,24 });
	leftAnim.PushBack({ 130,9,14,24 });
	leftAnim.PushBack({ 154,9,14,24 });
	leftAnim.PushBack({ 178,9,14,24 });
	leftAnim.loop = true;
	leftAnim.speed = 0.3f;

	upAnim.PushBack({ 9, 43,14,24 });
	upAnim.PushBack({ 33,43,14,24 });
	upAnim.PushBack({ 57,43,14,24 });
	upAnim.PushBack({ 81,43,14,24 });
	upAnim.PushBack({ 106,43,14,24 });
	upAnim.PushBack({ 130,43,14,24 });
	upAnim.PushBack({ 154,43,14,24 });
	upAnim.PushBack({ 178,43,14,24 });
	upAnim.loop = true;
	upAnim.speed = 0.3f;

	rightAnim.PushBack({ 211,9,14,24 });
	rightAnim.PushBack({ 235,9,14,24 });
	rightAnim.PushBack({ 259,9,14,24 });
	rightAnim.PushBack({ 280,9,14,24 });
	rightAnim.PushBack({ 307,9,14,24 });
	rightAnim.PushBack({ 331,9,14,24 });
	rightAnim.PushBack({ 355,9,14,24 });
	rightAnim.PushBack({ 379,9,14,24 });
	rightAnim.loop = true;
	rightAnim.speed = 0.3f;*/

	/*downAnim.PushBack({ 210,43,14,24 });
	downAnim.PushBack({ 234,43,14,24 });
	downAnim.PushBack({ 258,43,14,24 });
	downAnim.PushBack({ 282,43,14,24 });
	downAnim.PushBack({ 306,43,14,24 });
	downAnim.PushBack({ 330,43,14,24 });
	downAnim.PushBack({ 354,43,14,24 });
	downAnim.PushBack({ 378,43,14,24 });
	downAnim.loop = true;
	downAnim.speed = 0.3f;*/

}



ModulePlayer::~ModulePlayer()
{
}







bool ModulePlayer::Start()
{
	LOG("Loading player textures");

	bool ret = true;

	texture = app->tex->Load("Assets/Sprites/Natsu.png");
	
	currentAnimation = &idleLeftAnim;
	
	// Posici�n inicial (depende del lvl)
	position.x;
	position.y;
	 
	// X, Y, anchura, altura, 
	/*collider = App->collisions->AddCollider({ position.x-5, position.y, 24, 24 }, Collider::Type::PLAYER, this);*/

	
	
	return ret;
}


bool ModulePlayer::Update()
{


	if (playerMovement == true) {
		if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT ) ){		// mov Derecha
			nPosX = position.x + 1;
		}
		if ((app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT )) {		// mov arriba
			nPosY = position.y + 1;
		}
		if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT )) {		// mov izquierda
			nPosX = position.x - 1;
		}
		if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT ) ){		// mov abajo
			nPosY = position.y - 1;
		}
	}



	// player stop the animation when stop walking
	if (nPosX == 0 && nPosY == 0)
	{
		if (currentAnimation == &leftAnim)
		{
			if (currentAnimation != &idleLeftAnim) {
				idleLeftAnim.Reset();
				currentAnimation = &idleLeftAnim;
			}
		}
		if (currentAnimation == &upAnim) {
			if (currentAnimation != &idleUpAnim) {
				idleUpAnim.Reset();
				currentAnimation = &idleUpAnim;
			}
		}
		if (currentAnimation == &rightAnim) {
			if (currentAnimation != &idleRightAnim) {
				idleRightAnim.Reset();
				currentAnimation = &idleRightAnim;
			}
		}
		if (currentAnimation == &downAnim) {
			if (currentAnimation != &idleDownAnim) {
				idleDownAnim.Reset();
				currentAnimation = &idleDownAnim;
			}
		}
	}


	currentAnimation->Update();

	return true;
}

 bool ModulePlayer::PostUpdate()
{
	
	
	 SDL_Rect rect = currentAnimation->GetCurrentFrame();
	/* rect.h = 1035;
	 rect.w = 1194;
	 rect.x = 42;
	 rect.y = 55;*/
		app->render->DrawTexture(texture, position.x, position.y, &rect,1.0,0,0, 0);

	//SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY
	


	/*sprintf_s(scoreText, 10, "%4d", steps);
	App->fonts->BlitText(339, 24, scoreFont, scoreText);

	sprintf_s(limitText, 10, "%4d", limit);
	App->fonts->BlitText(339, 34, scoreFont, limitText);

	sprintf_s(stageText, 10, "%2d", stage);
	App->fonts->BlitText(356, 12, scoreFont, stageText);

	App->render->Blit(table, 310, 10, NULL);*/

	return true;
}
