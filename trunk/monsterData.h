#pragma once

#include "itemmanager.h" 
#include "coord.h"
#include "action.h"
#include "monster.h"
#include "spellmanager.h"

enum eMonsterState
{
	asleep,	//dont move, no detect
	sentry, //dont move, can detect
	normal, //irratic move, can detect
	hostile, //attacks/chases
	waking,  //transitional stage
	panic,	//
	dead
};

enum eMonsterGroup
{
	good,
	evil
};

struct itemData
{
	Item item;
	int equipped;

};

class monsterData
{
public:
	monsterData();
	~monsterData();

	//actions
	int		NextAction(Action *action);

	//get data
	int		Skill();
	int		Stamina();
	int		Luck()  { return monster.luck + luck_penalty; };
	int		Kills() { return kills; };
    std::string	Name();

	//Attack
	int		AdjustedSkill();
	int		AttackStrength();
	int		DefendStrength();
	bool    AbsorbTest();
	
	//tests
	int		isPlayer();
	int		isSeen();		//player can see monster //return 0 for not in radius, 1 for seen, 2 for hidden from view
    bool	isHumanoid()	{ return monster.humanoid; };
    bool	isUndead()	    { return monster.undead; };
	bool	isSpecial()		{return monster.special;};
	bool	wasHit()		{return monster.stamina==last_stamina ? false:true; }; //used for state change

	//other
	void	UpdateSightRange();
	void	Heal();
	int		Items();
	int		EquipKey(int level);
	int		TerrainAttack(int x, int y);
	void	SetState(eMonsterState newState){state=newState;};
	void	XP();
	eMonsterState	GetState() {return state;};
	coord * getPosition()	{return &pos;};

	//the monster
	Monster monster; //should be private - check: should this be a pointer??

	//monster data
	int				last_stamina;
	int				experience;
	int				experience_level;
	Action			action;
	//equipmmentSlots	slots;
	coord			pos;
	int				level;
	int				ref;
	ITEMLIST		inventory;
	MONSTERSPELLLIST	spellList;

	coord go_to;
	int is_archer;
	int is_magic;
	int TestArcher();
	bool miss_turn;

	//flee
	bool fleeing;
	int flee_count;
	int luck_penalty;
	int luck_counter;


private:	
	int kills;
	eMonsterState	state;
	eMonsterGroup	group;



};