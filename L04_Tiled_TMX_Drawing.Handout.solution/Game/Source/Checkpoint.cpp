//#include "Checkpoint.h"
//
//#include "App.h"
//#include "Textures.h"
//#include "Animation.h"
//#include "Input.h"
//#include "Render.h"
//#include "Window.h"
//#include "Audio.h"
//#include "Collider.h"
//#include "Scene.h"
//#include "ModulePlayer.h"
//#include "PathFinding.h"
//#include "ModulePhysics.h"
//
//#include "Map.h"
//#include "Defs.h"
//#include "Log.h"
//
//
//
//
//Checkpoint::Checkpoint() : Module()
//{
//	RFlag.PushBack({ 119, 5, 76, 128 });
//
//	BFlag.PushBack({ 13, 5, 76, 128 });
//}
//
//Checkpoint::~Checkpoint()
//{
//}
//
//bool Checkpoint::Start()
//{
//	bool ret = true;
////	Chpoint = app->tex->Load("Assets/Sprites/bandoleiro.png");
//	currentAnimation = &RFlag;
//
//	Chpointcoll = app->physics->AddCollider({ PChpoint.x ,PChpoint.y, 25 ,25 }, Collider::Type::CHECKPOINT, this);
//	return ret;
//}
//
//bool Checkpoint::Update(float dt)
//{
//
//	//app->render->DrawTexture(Chpoint, 0, 0, NULL);
//	if (checkpoint == false)
//	{
//		currentAnimation = &RFlag;
//	}
//
//	if (checkpoint == true)
//	{
//		if (BlueFlag == false)
//		{
//			currentAnimation = &BFlag;
//			BlueFlag = true;
//			saved = true;
//		}
//
//	}
//
//	if (saved == true)
//	{
//		app->SaveGameRequest();
//		saved = false;
//	}
//
//	return true;
//}
//
//bool Checkpoint::PostUpdate()
//{
//
//	return true;
//}
