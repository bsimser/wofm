#pragma once
#include "item.h"

enum eProjectileItemType //secondary type
{
	bow=1,
	longbow,
	crossbow=3,
	arrow=4,
	bolt=5
};

enum eProjectileItemCombo
{
	noLauncher=0,
	bowArrow,
	crossbowBolts
};

class ProjectileItem :
	public Item
{
public:
	ProjectileItem(void);
	virtual ~ProjectileItem(void);
	int CreateItem(eItemType type,int level,int secondary_type);

	int createProjectile(int level,int secondary_type);
	int createProjectileWeapon(int level,int secondary_type);
};
