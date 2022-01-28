#ifndef __ENTIYMANEGER_H__
#define __ENTIYMANEGER_H__


#include "Module.h"
#include "Entity.h"
#include "List.h"

#define MAX_ENTITIES 100

class Entity;

class EntityManager : public Module
{
public:
	EntityManager();

	virtual ~EntityManager();
	void Init();


	bool Awake(pugi::xml_node&);


	bool Start();


	bool Update(float dt);


	bool PostUpdate();


	bool CleanUp();


	Entity* CreateEntity(int x, int y, EntityType type, Entity* playerPointer = nullptr, EnemyType eType = EnemyType::NO_TYPE);
	void DestroyEntity(Entity* entity);


	bool UpdateAll(float dt, bool doLogic);


	void OnCollision(Collider* c1, Collider* c2);




	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

public:
	List<Entity*> entities;

	bool doLogic = false;

	int enemyDestroyedFx = 0;

}

#endif //!__MODULE_PLAYER_H__ 