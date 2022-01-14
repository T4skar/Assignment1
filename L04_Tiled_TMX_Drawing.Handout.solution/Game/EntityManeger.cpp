#include "EntityManeger.h"

#include "App.h"
#include "Module.h"
#include "Log.h"



EntityManeger::EntityManeger() : Module()
{
	//name.Create("entityManager");

}



EntityManeger::~EntityManeger()
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

}


bool EntityManager::CleanUp()
{

}



Entity* EntityManager::CreateEntity(int x, int y, EntityType type, Entity* playerPointer, EnemyType eType)
{

}


bool EntityManager::Update(float dt)
{
}


bool EntityManager::UpdateAll(float dt, bool doLogic)
{
}


bool EntityManager::PostUpdate()
{

}


void EntityManager::DestroyEntity(Entity* entity)
{

}



void EntityManager::OnCollision(Collider* c1, Collider* c2)
{

}

bool EntityManager::Load(pugi::xml_node& save)
{

}



bool EntityManager::Save(pugi::xml_node& save)
{
}
