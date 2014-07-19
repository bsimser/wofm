// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include "WorldBuilder.h"
#include ".\effects.h"


Effects::Effects(void) : max_effects(7)
{
}

Effects::~Effects(void)
{
}


void Effects::AddEffect(eEffect effect, int strength)
{
    if (effect < 0 || effect > max_effects)
        return;
    //if(effectMap[effect] < strength)
    //effectMap[effect] = strength;

    //test if already has effect;
    if (GetEffect(effect))
    {
        SetEffect(effect, strength);
    }
    else
    {
        EFFECT *e = new EFFECT;
        e->strength = strength;
        e->type = effect;
        effectList.push_back(*e);
    }
}

//set effects strength
int Effects::SetEffect(eEffect effect, int strength)
{
    EFFECTLIST::iterator eit;
    for (eit = effectList.begin(); eit != effectList.end(); eit++)
    {
        if (effect == eit->type) //if found return new strength
        {
            if (eit->strength < strength)
                eit->strength = strength;
            return strength;
        }
    }
    return 0;
}

void Effects::ReduceEffect(eEffect effect, int reduce)
{
    //effectMap[effect] -= reduce;

    //if(effectMap[effect] < 0) //cant be les than 0
    //	effectMap[effect] =0;

    //if(effectMap[effect] == 0)
    //	RemoveEffect(effect);

    EFFECTLIST::iterator it;
    int size = effectList.size();
    int i = 0;
    for (it = effectList.begin(); it != effectList.end(); it++, i++)
    {
        int go = i;
        int t = it->type;
        int s = it->strength;

        if (effect == it->type)
        {
            it->strength -= reduce;
            if (it->strength <= 0)
            {
                //		RemoveEffect(effect);
                return;;
            }
        }
    }

}
void Effects::RemoveEffect(eEffect effect)
{
    EFFECTLIST::iterator eit;
    for (eit = effectList.begin(); eit != effectList.end(); eit++)
    {
        if (effect == eit->type)
        {
            effectList.erase(eit);
            break;
        }
    }
}

int Effects::GetEffect(eEffect effect)
{
    EFFECTLIST::iterator eit;
    for (eit = effectList.begin(); eit != effectList.end(); eit++)
    {
        if (effect == eit->type)
            return eit->strength;

    }
    return 0;

    //return effectMap[effect];
}