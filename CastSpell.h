#pragma once

class CastMagic
{
public:
	int SlowEnemies(MonsterData* caster);
	int RepelMissiles(MonsterData* caster);
	int Teleport(MonsterData* caster);
	int FlyingWeapon(MonsterData* caster,int x,int y);
	int DragonBreath(MonsterData* caster,int x,int y);

	int CalculateDamage(int attackStr, int defenceStr);
};
