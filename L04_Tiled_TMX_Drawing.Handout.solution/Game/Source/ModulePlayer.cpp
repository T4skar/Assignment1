#include "ModulePlayer.h"
#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Audio.h"
#include "Defs.h"
#include "Log.h"
#include "Animation.h"


//#include "ModuleCollisions.h"


#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h> 
#include "Scene.h"
//#include "ModuleBox.h"

ModulePlayer::ModulePlayer() : Module()
{

	// Aqui van las animaciones del player
	idleRightAnim.PushBack({ 1050,670,110,170 });
	idleRightAnim.PushBack({ 1164,670,110,170 });
	idleRightAnim.PushBack({ 1278,670,110,170 });
	idleRightAnim.PushBack({ 1400,670,110,170 });
	idleRightAnim.loop = true;
	idleRightAnim.speed = 0.003f;

	rightAnim.PushBack({ 1657,690,124,135 });
	rightAnim.PushBack({ 1797,684,124,135 });
	rightAnim.PushBack({ 1915,684,124,135 });
	rightAnim.PushBack({ 2060,684,124,135 });
	rightAnim.PushBack({ 2196,684,124,135 });
	rightAnim.PushBack({ 2310,684,124,135 });
	rightAnim.loop = true;
	rightAnim.speed = 0.03f;

	/*upAnim.PushBack({ 9, 43,14,24 });
	upAnim.PushBack({ 33,43,14,24 });
	upAnim.PushBack({ 57,43,14,24 });
	upAnim.PushBack({ 81,43,14,24 });
	upAnim.PushBack({ 106,43,14,24 });
	upAnim.PushBack({ 130,43,14,24 });
	upAnim.PushBack({ 154,43,14,24 });
	upAnim.PushBack({ 178,43,14,24 });
	upAnim.loop = true;
	upAnim.speed = 0.3f;*/

	leftAnim.PushBack({ 211,9,14,24 });
	leftAnim.PushBack({ 235,9,14,24 });
	leftAnim.PushBack({ 259,9,14,24 });
	leftAnim.PushBack({ 280,9,14,24 });
	leftAnim.PushBack({ 307,9,14,24 });
	leftAnim.PushBack({ 331,9,14,24 });
	leftAnim.PushBack({ 355,9,14,24 });
	leftAnim.PushBack({ 379,9,14,24 });
	leftAnim.loop = true;
	leftAnim.speed = 0.03f;

	

}



ModulePlayer::~ModulePlayer()
{
}







bool ModulePlayer::Start()
{
	LOG("Loading player textures");

	bool ret = true;

	texture = app->tex->Load("Assets/Sprites/Natsu3.png");
	
	currentAnimation = &idleRightAnim;
	
	// Posición inicial (depende del lvl)

	 
	// X, Y, anchura, altura, 
	/*collider = App->collisions->AddCollider({ position.x-5, position.y, 24, 24 }, Collider::Type::PLAYER, this);*/

	
	
	return ret;
}


bool ModulePlayer::Update(float dt)
{


	
		if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT )){		// mov Derecha
			 position.x += 1;
			
				if (currentAnimation != &rightAnim) {
					rightAnim.Reset();
					currentAnimation = &rightAnim;
				}
			
		}
		
		else if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT )) {		// mov izquierda
			 position.x -= 1;
			
			
				if (currentAnimation != &leftAnim) {
					leftAnim.Reset();
					currentAnimation = &leftAnim;
				}
			
		}
		else  {
			if (currentAnimation != &idleRightAnim) {
				idleRightAnim.Reset();
				currentAnimation = &idleRightAnim;
			}
		}
		//if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && nPosX == 0 && nPosY == 0)){		// mov abajo
		//	nPosY = position.y - 1;
		//}
	



	// player stop the animation when stop walking

	
		
		if (currentAnimation == &upAnim) {
			if (currentAnimation != &idleUpAnim) {
				idleUpAnim.Reset();
				currentAnimation = &idleUpAnim;
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
		app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,0,0, 0);

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
