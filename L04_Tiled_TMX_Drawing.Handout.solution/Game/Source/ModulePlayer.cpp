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
#include <stdio.h> // Camarada
#include "Scene.h"
//#include "ModuleBox.h"

/*ModulePlayer::ModulePlayer(bool startEnabled) :Module(startEnabled)
{

	// Aqui van las animaciones del player
	idleLeftAnim.PushBack({ 9,9,14,24 });
	idleUpAnim.PushBack({ 9,43,14,24 });
	idleRightAnim.PushBack({ 211,9,14,24 });
	idleDownAnim.PushBack({ 210,43,14,24 });

	leftAnim.PushBack({ 9,9,14,24 });
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
	rightAnim.speed = 0.3f;

	downAnim.PushBack({ 210,43,14,24 });
	downAnim.PushBack({ 234,43,14,24 });
	downAnim.PushBack({ 258,43,14,24 });
	downAnim.PushBack({ 282,43,14,24 });
	downAnim.PushBack({ 306,43,14,24 });
	downAnim.PushBack({ 330,43,14,24 });
	downAnim.PushBack({ 354,43,14,24 });
	downAnim.PushBack({ 378,43,14,24 });
	downAnim.loop = true;
	downAnim.speed = 0.3f;

	boxleftAnim.PushBack({ 9,75,16,21 });
	boxleftAnim.PushBack({ 33,76,17,20 });
	boxleftAnim.PushBack({ 57,76,18,20 });
	boxleftAnim.PushBack({ 81,75,17,21 });
	boxleftAnim.PushBack({ 105,76,16,20 });
	boxleftAnim.PushBack({ 129,76,17,20 });
	boxleftAnim.loop = true;
	boxleftAnim.speed = 0.2f;

	boxupAnim.PushBack({ 11,104,12,21 });
	boxupAnim.PushBack({ 35,104,12,21 });
	boxupAnim.PushBack({ 59,104,12,22 });
	boxupAnim.PushBack({ 83,104,12,24 });
	boxupAnim.PushBack({ 107,104,12,21 });
	boxupAnim.PushBack({ 131,104,12,21 });
	boxupAnim.PushBack({ 155,104,12,22 });
	boxupAnim.PushBack({ 179,104,12,24 });
	boxupAnim.loop = true;
	boxupAnim.speed = 0.2f;

	boxdownAnim.PushBack({ 211,104,13,24 });
	boxdownAnim.PushBack({ 235,104,13,24 });
	boxdownAnim.PushBack({ 259,104,13,24 });
	boxdownAnim.PushBack({ 283,104,13,24 });
	boxdownAnim.PushBack({ 307,104,13,24 });
	boxdownAnim.PushBack({ 331,104,13,24 });
	boxdownAnim.PushBack({ 355,104,13,24 });
	boxdownAnim.PushBack({ 379,104,13,24 });
	boxdownAnim.loop = true;
	boxdownAnim.speed = 0.2f;

	boxrightAnim.PushBack({ 204,75,19,21 });
	boxrightAnim.PushBack({ 228,75,19,21 }); 
	boxrightAnim.PushBack({ 252,75,19,21 });
	boxrightAnim.PushBack({ 276,75,19,21 });
	boxrightAnim.PushBack({ 300,75,19,21 }); 
	boxrightAnim.PushBack({ 324,75,19,21 });
	boxrightAnim.PushBack({ 348,75,19,21 });
	boxrightAnim.PushBack({ 372,75,19,21 });

	boxrightAnim.loop = true;
	boxrightAnim.speed = 0.2f;
}*/

ModulePlayer::~ModulePlayer()
{

}




bool ModulePlayer::Start()
{
	LOG("Loading player textures");

	bool ret = true;

	texture = app->tex->Load("Assets/Sprites/Natsu.png");
	/*table= App->textures->Load("assets/UI/table.png");
	currentAnimation = &idleLeftAnim;*/
	
	// Posición inicial (depende del lvl)
	position.x;
	position.y;

	// X, Y, anchura, altura, 
	/*collider = App->collisions->AddCollider({ position.x-5, position.y, 24, 24 }, Collider::Type::PLAYER, this);

	char lookupTable[] = { "0123456789 0123456789" };
	scoreFont = App->fonts->Load("assets/fonts/font1.png", lookupTable, 2);
	*/
	return ret;
}


/*Scene ModulePlayer::Update(float dt)
{
	

	if (playerMovement == true) {
		if ((app->input->Getkeys(SDL_SCANCODE_D) == KEY_REPEAT ) {		// mov Derecha
			nPosX = position.x + 1;
		}
		if ((app->input->Getkey(SDL_SCANCODE_W) == KEY_REPEAT ) {		// mov arriba
			nPosY = position.y + 1;
		}
		if ((app->input->Getkey(SDL_SCANCODE_A) == KEY_REPEAT ) {		// mov izquierda
			nPosX = position.x - 1;
		}
		if ((app->input->Getkey(SDL_SCANCODE_S) == KEY_REPEAT ) {		// mov abajo
			nPosY = position.y - 1;
		}
	}*/

	

	// player stop the animation when stop walking
	/*if (nPosX == 0 && nPosY == 0) 
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

	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate()
{
	
	if (IsEnabled())
	{
		SDL_Rect rect = currentAnimation->GetCurrentFrame();
		App->render->Blit(texture, position.x, position.y, &rect);
	}
	else {
		steps = 0000;
		limit = 00;
		stage = 00;
		delete collider;
	}*/


	/*sprintf_s(scoreText, 10, "%4d", steps);
	App->fonts->BlitText(339, 24, scoreFont, scoreText);

	sprintf_s(limitText, 10, "%4d", limit);
	App->fonts->BlitText(339, 34, scoreFont, limitText);

	sprintf_s(stageText, 10, "%2d", stage);
	App->fonts->BlitText(356, 12, scoreFont, stageText);

	App->render->Blit(table, 310, 10, NULL);*/

	/*return update_status::UPDATE_CONTINUE;
}*/
