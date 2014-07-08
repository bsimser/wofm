#pragma once

class CastMagic
{
public:
	int SlowEnemies(monsterData* caster);
	int RepelMissiles(monsterData* caster);
	int Teleport(monsterData* caster);
	int FlyingWeapon(monsterData* caster,int x,int y);
	int DragonBreath(monsterData* caster,int x,int y);

	int CalculateDamage(int attackStr, int defenceStr);
};
