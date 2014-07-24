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
#include ".\effectmanager.h"

EffectManager::EffectManager(void)
{
}

EffectManager::~EffectManager(void)
{
}

int EffectManager::RunEffect(MonsterData *monster, eEffect effect, int strength)
{
    if (strength == 0)
        return 0;

    int ret = 0;

    switch (effect)
    {
    case no_effect: break;
    case poisoned: return RunPoison(monster, strength); break;
    case repelMissiles:
        ret = TestEffect(monster, effect, strength);
        if (monster->isPlayer())
        {
            if (ret == REDUCED)
                World.getTextManager().newLine("Your glow fades slightly. ");
            else if (ret == REMOVED)
                World.getTextManager().newLine("You stop glowing. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s stops glowing. ", monster->monster.name.c_str());
        break;
    case diseased:
        ret = TestEffect(monster, effect, strength);
        if (monster->isPlayer())
        {
            if (ret == REDUCED)
                World.getTextManager().newLine("You feel healthier. ");
            else if (ret == REMOVED)
                World.getTextManager().newLine("You feel better. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s looks better. ", monster->monster.name.c_str());
        break;

    case slowed:
        ret = TestEffect(monster, effect, strength);
        if (monster->isPlayer())
        {
            if (ret == REDUCED)
                World.getTextManager().newLine("You speed up slightly. ");
            else if (ret == REMOVED)
                World.getTextManager().newLine("You speed up. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s speeds up. ", monster->monster.name.c_str());

        break;

    case paralysis:
        ret = TestEffect(monster, effect, strength);
        if (monster->isPlayer())
        {
            if (ret == REDUCED)
                World.getTextManager().newLine("You feel movement. ");
            else if (ret == REMOVED)
                World.getTextManager().newLine("You can move again. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s can move again. ", monster->monster.name.c_str());
        break;

    case teleportitus: ret = TestEffect(monster, effect, strength);
        if (ret != REMOVED)
            RunTeleport(monster, strength);
        if (monster->isPlayer())
        {
            //if(ret == REDUCED)
            //	World.getTextManager().newLine("You feel more stable. ");
            if (ret == REMOVED)
                World.getTextManager().newLine("Your feel stable. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s looks stable. ", monster->monster.name.c_str());
        break;


    case confused:
        ret = TestEffect(monster, effect, strength);
        if (monster->isPlayer())
        {
            if (ret == REDUCED)
                World.getTextManager().newLine("You feel less confused. ");
            else if (ret == REMOVED)
                World.getTextManager().newLine("Your head clears. ");
        }
        else  if (monster->isSeen() && ret == REMOVED)
            World.getTextManager().newLine("The %s looks normal. ", monster->monster.name.c_str());
        break;
    }

    return ret;
}


int EffectManager::TestGetEffect(MonsterData *monster, eBrandType brand, int strength)
{
    int monster_resistance = 0;

    if (monster->monster.hasResistance()) //get natural resistance
    {
        monster_resistance = monster->monster.GetResistance(brand);
        //resList.push_back(monster_resistance);
    }

    int found_all = 0;

    ITEMLIST::iterator it;

    //do  //search through equipment
    //{
    int highest_res = monster_resistance;
    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++)
    {
        int res = it->GetResistance(brand);
        if (res > highest_res)
            highest_res = res;
    }
    //}while (!found_all);

    //float modifier =  1-(highest_res)*0.3f;
    //new_damage = (int)((damage* modifier)+.5f);

    int hit = Random::getInt(10, 0);
    int avoid = Random::getInt(10, 0);

    if (hit*(strength) > avoid*(highest_res + 1) && !Random::getInt(3, 0)) //+33% chance to tone it down
        return 1;

    return 0;

}

void EffectManager::AddBrandEffect(MonsterData *monster, eBrandType brand, int strength)
{
    switch (brand)
    {
    case bPoison: monster->monster.AddEffect(poisoned, strength); break;
    case bParalysis: monster->monster.AddEffect(paralysis, strength);     break;     	
        if(monster->isPlayer())
           World.getTextManager().newLine("You are paralysed. ");
        else if(monster->isSeen())
           World.getTextManager().newLine("The %s is paralysed. ",monster->monster.name.c_str());;

        break;
    }


}

void EffectManager::AddOtherEffect(MonsterData *monster, eEffect effect, int strength) //spell/potion/scroll
{
    monster->monster.AddEffect(effect, strength);
    /*switch(effect)
    {
    case bPoison: monster->monster.AddEffect(poisoned,strength);
    break;
    }*/
}

int EffectManager::TestEffect(MonsterData *monster, eEffect effect, int strength)
{
    int ret = NO_CHANGE;

    //get Resistance
    int resistance = monster->monster.GetResistance(EquivalentResistance(effect));

    //determine reducement
    int chance = Random::getInt(10, 0);
    if (chance < resistance + 1)//10% chance reduction + 10% for each resistance point - 0= 10%
    {
        monster->monster.ReduceEffect(effect, 1);
        ret = REDUCED;
    }

    //determine effect
    if (monster->monster.GetEffect(effect) == 0)
        ret = REMOVED;

    return ret;
}

int EffectManager::RunSlowed(MonsterData *monster, int strength)
{
    //get Resistance
    int resistance = monster->monster.GetResistance(bSlow);

    //determine reducement
    int chance = Random::getInt(10, 0);
    if (chance < resistance + 1)//10% chance reduction + 10% for each resistance point - 0= 10%
    {
        if (monster->isPlayer())
            World.getTextManager().newLine("You speed up slightly. ");
        monster->monster.ReduceEffect(slowed, 1);
    }

    //determine effect
    if (monster->monster.GetEffect(slowed) > 0)
    {
        // nothing for slowed
        return 1;
    }
    else
    {
        if (monster->isPlayer())
            World.getTextManager().newLine("You speed up. ");
        else if (monster->isSeen())
            World.getTextManager().newLine("The %s speeds up. ", monster->monster.name.c_str());
        return 0;
    }

}
int EffectManager::RunPoison(MonsterData *monster, int strength)
{
    unsigned char c1 = monster->monster.color1;
    unsigned char c2 = monster->monster.color2;
    unsigned char c3 = monster->monster.color3;
    monster->monster.setColour(0, 255, 0);
    World.RenderScene();
    Sleep(20);
    monster->monster.setColour(c1, c2, c3);

    World.RenderScene();

    //get Resistance
    int resistance = monster->monster.GetResistance(EquivalentResistance(poisoned));

    //determine reducement
    int chance = Random::getInt(10, 0);
    if (chance < resistance + 1)//10% chance reduction + 10% for each resistance point - 0= 10%
    {
        monster->monster.ReduceEffect(poisoned, 1);
        if (monster->isPlayer())
            World.getTextManager().newLine("You feel less poisoned. ");
        return REDUCED;
    }

    //determine effect
    if (monster->monster.GetEffect(poisoned) > 0)
    {
        int damage = Random::getInt(10, 0);
        if (damage > resistance + 5) //50% take damage + 10% for each resistance point
        {
            monster->monster.stamina--;

            if (monster->isPlayer())
            {
                World.getTextManager().newLine("You are poisoned. ");
                World.getDeathMessage().SetDeathMessage("became immune to poison the moment he died of it. ");
            }
            else if (monster->isSeen())
                World.getTextManager().newLine("The %s is poisoned. ", monster->monster.name.c_str());
        }
    }
    else
    {
        if (monster->isPlayer())
            World.getTextManager().newLine("You are no longer poisoned. ");
        else if (monster->isSeen())
            World.getTextManager().newLine("The %s recovers. ", monster->monster.name.c_str());
        return REMOVED;
    }
    return NO_CHANGE;
}

void EffectManager::RunTeleport(MonsterData *monster, int strength)
{
    int resistance = monster->monster.GetResistance(EquivalentResistance(teleportitus));

    int chance = Random::getInt(100, 0) + strength * 2;
    if (chance > 98)
    {
        int range = 10 * strength;
        //Teleport(monster,range);
    }
}

eBrandType EffectManager::EquivalentResistance(eEffect effect)
{
    switch (effect)
    {
    case poisoned:		return bPoison;
    case diseased:		return bDisease;
    case slowed:		return bSlow;
    case paralysis:		return bParalysis;
    case teleportitus:	return bDisplacment;
    case confused:		return bConfusion;
    default:			return bNone;
    }
}

const std::string EffectManager::EffectName(eEffect effect)
{
    switch (effect)
    {
    case poisoned:			return "Posioned";
    case diseased:			return "Diseased";
    case slowed:			return "Slowed";
    case paralysis:			return "Paralysed";
    case teleportitus:		return "Teleports";
    case confused:			return "Confused";
    case repelMissiles:		return "Repel Missiles";
    default:				return "Error";
    }
}