// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>


enum eEffect
{
	no_effect,
	poisoned,
	diseased,
	slowed,
	paralysis,
	teleportitus,
	confused,
	repelMissiles
};
typedef struct
{
	eEffect type;
	int strength;
}EFFECT;

//typedef std::map<eEffect,int> EFFECTMAP;

typedef std::vector<EFFECT> EFFECTLIST;

//typedef EFFECTMAP::const_iterator EFFECTMAP_CITERATOR;

//class to keep track of monster effects

class Effects
{
public:
	Effects(void);
	~Effects(void);

    void AddEffect(eEffect effect, int strength);
	int  GetEffect(eEffect effect);
	int  SetEffect(eEffect effect,int strength);
	void ReduceEffect(eEffect effect, int reduce);
	void RemoveEffect(eEffect effect);

	//EFFECTMAP effectMap;
	EFFECTLIST effectList;

private:
	int max_effects;

};
