#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "SDL/include/SDL_Rect.h"

class Module;
#define MAX_LISTENERS 1000
struct Collider
{
	enum Type
	{
		NONE = -1,
		PLAYER,
		MATAR,
		CORAZON,
		COIN,
		COIN2,
		RIGHT,
		LEFT,
		UP,
		ENEMY,
		ENEMYR,
		ENEMYL,
		ENEMYV,
		ENEMYVR,
		ENEMYVL,
		ENEMYUP,
		ENEMYDOWN,
		GROUND,
		DEAD,
		WIN,
		CHECKPOINT,
		CHECKPOINT2,
		TELEPORT,

		MAX
	};

	//Methods
	Collider(SDL_Rect rectangle, Type type, Module* listener = nullptr);

	void SetPos(int x, int y);

	bool Intersects(const SDL_Rect& r) const;

	//Variables
	SDL_Rect rect;
	bool pendingToDelete = false;
	Type type;
	Module* listeners[MAX_LISTENERS] = { nullptr };
};


#endif // !__COLLIDER_H__

