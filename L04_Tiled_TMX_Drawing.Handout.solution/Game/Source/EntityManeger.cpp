#include "EntityManeger.h"

#include "App.h"
#include "Module.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Map.h"
#include"ModulePhysics.h"
#include "SceneCredits.h"
#include "ModuleEnemy.h"
#include "SceneTitle.h"
#include "ModuleEnemyVolador.h"
#include "ModulePlayer.h"
#include "Corazones.h"
#include "Animation.h"
#include"PerfTimer.h"
#include "Checkpoint.h"
#include "Timer.h"
#include"ModuleCoin.h"
#include "ModuleFadeToBlack.h"

#include "List.h"

#include "Defs.h"
#include "Log.h"



EntityManager::EntityManager():Module()
{
	name.Create("entityManager");

}

EntityManager::~EntityManager()
{

}



void EntityManager::Init()
{
	active = false;

}

bool EntityManager::Awake(pugi::xml_node& config)
{
}



bool EntityManager::Start()
{
	

	doLogic = true;
	
	
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		ListItem<Entity*>* eNext = e->next;
		DestroyEntity(e->data);
		e = eNext;
	}
	entities.clear();

	return true;
}


bool EntityManager::CleanUp()
{
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		ListItem<Entity*>* eNext = e->next;
		DestroyEntity(e->data);
		e = eNext;
	}
	entities.clear();



	/*app->tex->UnLoad(playerTex);
	app->tex->UnLoad(playerHeart);
	app->tex->UnLoad(ground);
	app->tex->UnLoad(flying);
	app->tex->UnLoad(tileSetTex);*/

}



Entity* EntityManager::CreateEntity(int x, int y, EntityType type, Entity* playerPointer, EnemyType eType)
{
	Entity* ret = nullptr;

	switch (type)
	{
	case EntityType::PLAYER:
	{
		ret = new ModulePlayer(x, y);
		break;
	}
	case EntityType::ENEMY:
	{
		switch (eType)
		{
		case EnemyType::SUELO:
		{
			ret = new ModuleEnemy(x, y, eType, playerPointer);
			break;
		}
		default:
			break;
		}
		break;
	}
	case EntityType::COIN:
	{
		ret = new ModuleCoin(x, y);
		break;
	}
	case EntityType::HEART:
	{
		ret = new Corazones(x, y);
		break;
	}
	default:
	{
		break;
	}
	}

	if (ret != nullptr)
	{
		entities.add(ret);
	}

	return ret;

}


bool EntityManager::Update(float dt)
{
	return true;
}


bool EntityManager::UpdateAll(float dt, bool doLogic)
{
	return true;
}


bool EntityManager::PostUpdate()
{
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		if (e->data->pendingToDelete == true)
		{
			DestroyEntity(e->data);
		}
		else
		{
			e->data->Draw();
		}
		e = e->next;
	}


	return true;
}


void EntityManager::DestroyEntity(Entity* entity)
{
	if (entity->collider != nullptr)
	{
		entity->collider->pendingToDelete = true;
	}
	int i = entities.find(entity);
	delete entities[i];
	entities.del(entities.At(i));
}



