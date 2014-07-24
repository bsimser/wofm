// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma warning(disable : 4786) 

#include "MonsterAI.h"
#include "WorldBuilder.h"

#include "NumberGenerator.h"
#include "EffectManager.h"


#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MonsterData* current_monster;

int MonsterAI::ProcessIntelligence(MonsterData* monster)
{
    current_monster = monster;

    if (ProcessEffects(monster)) //effect took control of action
        return monster->GetState();

    if (monster->isPlayer())
        return 0; //player no AI

    if (monster->monster.GetType() == mRandom && monster->Name() == "rat")
    {
        if (ProcessRat(monster))
            return 1;
        else if (!Detect(monster) && Random::getInt(4,0) != 0)
        {
            return DoNothing(monster);
        }
    }

    Detect(monster);

    UpdateMonsterState(monster);

    DetermineAction(monster);

    return monster->GetState();
}

int MonsterAI::ProcessEffects(MonsterData* monster)
{
    EFFECTLIST::iterator it;

    EffectManager effectManager;
    int move_done = 0;

    int size = monster->monster.effectList.size();
    for (it = monster->monster.effectList.begin(); it != monster->monster.effectList.end();)
    {
        if (effectManager.RunEffect(monster, it->type, it->strength) == REMOVED ||  it->strength <= 0)
        {
            it = monster->monster.effectList.erase(it);
        }
        else
        {
            move_done |= EffectAction(monster, it->type, it->strength);
            it++;
        }
    }
    return move_done;
    /*//EFFECTMAP_CITERATOR it;
    EffectManager effectManager;

    int move_done=0;

    //constant look
    int i=0;
    for(it = monster->monster.effectMap.begin();it !=monster->monster.effectMap.end();it++,i++)
    {
    effectManager.RunEffect(monster,it->first, it->second);
    move_done |= EffectAction(monster,it->first,it->second);
    if(i > 7) //temp fix bug
    return move_done;
    }*/

    return move_done; //if > 0 skip move
}

int MonsterAI::ProcessRat(MonsterData* monster)
{
    int x = 0;
    int y = 0;
    if (DetectCheese(monster->pos.x, monster->pos.y, x, y))
    {
        if (monster->GetState() == asleep && Random::getInt(5, 0) == 0)
        {
            monster->SetState(normal);
        }
        else if (x != 0 && y != 0)
        {
            if (!MoveCloserToPlayer(monster->pos.x, monster->pos.y, x, y))
                return RandomMove(monster);
            return 1;
        }
    }
    return 0;
}

int	MonsterAI::EffectAction(MonsterData* monster, eEffect effect, int strength)
{
    if (strength == 0)
        return 0;

    int move_done = 0;

    switch (effect)
    {
    case slowed:	if (monster->isPlayer())
    {
        monster->miss_turn = !monster->miss_turn;; return 0;
    }
    monster->miss_turn = !monster->miss_turn;
    if (monster->miss_turn)
    {
        move_done = 1;

    }break; //skip every second move

    case paralysis: move_done = 2;
        if (monster->isPlayer())
            World.getTextManager().newLine("You cannot move. "); break;

    case confused: if (Random::getInt(10, 0) > 3 - strength)
    {
        RandomMove(monster);
        move_done = 1;
        if (monster->isPlayer())
        {
            World.getTextManager().newLine("You are confused. ");
        }
        else if (monster->isSeen() == 1)
            World.getTextManager().newLine("%s is confused. ", monster->monster.name.c_str());

    } break; //70%+10% for strength random move;

    case teleportitus: 
        if (Random::getInt(20, 0) < 0 + strength)
        {
            /*Teleport();*/ move_done = 1;
        }
        break;
    }
    return  move_done;
}

int	MonsterAI::DetermineAction(MonsterData* monster)
{
    if (monster->GetState() == normal)
    {
        RandomMove(monster);
    }

    else if (monster->GetState() == sentry || monster->GetState() == asleep)
    {
        DoNothing(monster);
    }

    else if (monster->GetState() == hostile)
    {
        AttackPlayer(monster);
        //ChasePlayer(monster);
    }

    return 1;
}

int MonsterAI::AttackPlayer(MonsterData* monster)
{
    if (monster->is_magic)
        MagicAttackPlayer(monster);
    else if (monster->is_archer)
        DistanceAttackPlayer(monster);
    else
    {
        if (monster->Name() == "ogre" && World.getMonsterManager().getMonsterItems().GetInventoryItem(monster, carcass))
        {
            if (monster->isSeen())
            {
                return World.getActionManager().monsterAction.ThrowItem(monster,
                                                                        World.getMonsterManager().Player()->pos.x,
                                                                        World.getMonsterManager().Player()->pos.y,
                                                                        World.getMonsterManager().getMonsterItems().GetInventoryItem(monster, carcass)->ref);
            }
        }
        else
            ChasePlayer(monster);
    }

    return 0;
}
int MonsterAI::Detect(MonsterData* monster)
{
    //detect player

    detect_timeout = 0;
    int x;
    int y;

    detect = DetectPlayer(monster->pos.x, monster->pos.y, &x, &y);

    if (monster->pos.x == monster->go_to.x && monster->pos.y == monster->go_to.y) //lost player
        detect_timeout = 1;

    return detect;
}

eMonsterState MonsterAI::UpdateMonsterState(MonsterData* monster)
{
    eMonsterState state = monster->GetState();

    //run state machine

    if (state == normal)
    {
        if (detect)
        {
            state = hostile;
        }
    }
    else if (state == waking)
    {
        state = hostile;

    }
    else if (state == hostile)
    {
        if (detect_timeout)
        {
            state = normal;
        }
    }
    else if (state == asleep)
    {
        if (detect || monster->wasHit())
        {
            if (monster->wasHit() == false && World.getMonsterManager().Player()->TestLuck())
            {
                World.getTextManager().newLine("The %s snores loudly. ", monster->monster.name.c_str());
            }
            else
            {
                state = waking;
                World.getTextManager().newLine("The %s wakes up. ", monster->monster.name.c_str());
            }
        }
    }
    else if (state == sentry)
    {
        if (detect)
        {
            state = hostile;
        }
    }

    //update monster state
    monster->SetState(state);

    return monster->GetState();
}

int MonsterAI::DoNothing(MonsterData* monster)
{
    if (monster->GetState() != dead)
    {
        monster->NextAction(World.getActionManager().UpdateAction(&monster->action, aWait));
    }
    return 1;
}

int MonsterAI::RandomMove(MonsterData* monster)
{
    //if(monster->ref ==0)  //no player commands
    //	return 0;

    if (monster->GetState() == asleep) return 0;

    int dir = Random::getInt(9, 1);

    Coord * pos = monster->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;

    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:	 new_pos.x++; break;
    case dWest:	 new_pos.x--; break;
    case dSouth: new_pos.y++; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;
    }

    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    //cant move into stone, dont attack friendly monster, dont willingly go into water.

    if (dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.type != stone &&
         dungeonLevel->getCell(new_pos.x, new_pos.y).GetMonster() == NULL &&
        (dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.type != deepWater || monster->Name() == "crocodile"))
    {
        //	if(monster->state != dead && dungeonLevel->getCell(pos->x, pos->y).terrain.type != deepWater) 
        {
            dungeonLevel->getCell(pos->x, pos->y).RemoveMonsterRef();
            monster->NextAction(World.getActionManager().UpdateAction(&monster->action, aMove, new_pos.x, new_pos.y));
        }
        //else get out of water
    }
    else if (dungeonLevel->getCell(pos->x, pos->y).terrain.type == deepWater && dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.type != stone)
    {	//find a way out of deep water
        dungeonLevel->getCell(pos->x, pos->y).RemoveMonsterRef();
        monster->NextAction(World.getActionManager().UpdateAction(&monster->action, aMove, new_pos.x, new_pos.y));
    }
    // dig your way out
    else if (dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.type == stone &&
             monster->monster.GetType() == mDigger)
    {
        if (Random::getInt(3,0) == 1 && new_pos.x < DUNGEON_SIZE_W - 1 && new_pos.y < DUNGEON_SIZE_H - 1 && new_pos.x > 0 && new_pos.y > 0)
        {
            dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.Create(dfloor);
            monster->NextAction(World.getActionManager().UpdateAction(&monster->action, aMove, new_pos.x, new_pos.y));
        }
    }
    return 1;
}

int MonsterAI::MagicAttackPlayer(MonsterData* monster)
{
    int p_x;
    int p_y;
    int m_x = monster->pos.x;
    int m_y = monster->pos.y;

    if (Random::getInt(10, 0) < 2 || (monster->monster.name ==  "dragon" && Random::getInt(10, 0) < 5)) //no spells 20% of the time
    {
        return ChasePlayer(monster);
    }

    if (DetectPlayer(monster->pos.x, monster->pos.y, &p_x, &p_y))//if found
    {
        float range = sqrt((float)(p_x - m_x)*(p_x - m_x) + (m_y - p_y)*(m_y - p_y));
        if (range > monster->monster.sight_range*0.75 || range < 2) //set ideal range
        {
            return ChasePlayer(monster);
        }

        //search spell list and come up with something good
        //int spell_choice = GetSpellChoice();

        //for now use random

        int random_spell = Random::getInt(monster->spellList.size(), 0);

        SpellBase *spell = World.getSpellManager().GetMonsterSpell(monster, random_spell);
        if (!spell)
            ChasePlayer(monster);
        else if (spell->GetTarget() == spTarget) //target spell
        {
            World.getActionManager().monsterAction.CastSpell(monster, random_spell);//prepare spell

            if (spell->GetType() == spAttack) //player target
                World.getActionManager().monsterAction.CastTargetSpell(monster, spell->GetSpell(), p_x, p_y);
            else if (spell->GetType() == spDefence || spell->GetType() == spEnhance || spell->GetType() == spPanic) //target self
                World.getActionManager().monsterAction.CastTargetSpell(monster, spell->GetSpell(), m_x, m_y);
        }
        else if (spell->GetTarget() == spAreaEffect) //area spell
            World.getActionManager().monsterAction.CastSpell(monster, random_spell);

        if (m_x != monster->pos.x || m_y != monster->pos.y) //caster has moved
            DetectPlayer(monster->pos.x, monster->pos.y, &p_x, &p_y);

        //set last known player Coord
        monster->go_to.x = p_x;
        monster->go_to.y = p_y;
    }
    else
    {
        return ChasePlayer(monster);
    }

    return 1;
}

int MonsterAI::DistanceAttackPlayer(MonsterData* monster)
{
    int p_x;
    int p_y;
    int m_x = monster->pos.x;
    int m_y = monster->pos.y;

    if (DetectPlayer(monster->pos.x, monster->pos.y, &p_x, &p_y))//if found
    {
        float range = sqrt((float)(p_x - m_x)*(p_x - m_x) + (m_y - p_y)*(m_y - p_y));
        if (range > monster->monster.sight_range*0.75 || range < 2) //set ideal range
        {
            return ChasePlayer(monster);
        }
        if (!World.getActionManager().monsterAction.ThrowTarget(monster, p_x, p_y, aFire)) //change to nextAction command
            ChasePlayer(monster);

        //p_x,p_y; set last know player Coord to these
        monster->go_to.x = p_x;
        monster->go_to.y = p_y;
    }
    else
    {
        return ChasePlayer(monster);
    }

    return 1;
}
int MonsterAI::ChasePlayer(MonsterData* monster)
{
    int sight_range = 10;
    int m_x, m_y; //monster Coord
    int p_x, p_y; //player Coord 

    m_x = monster->pos.x;
    m_y = monster->pos.y;

    //find player

    if (DetectPlayer(m_x, m_y, &p_x, &p_y))//if found
    {
        MoveCloserToPlayer(m_x, m_y, p_x, p_y);	//move toward player

        //p_x,p_y; set last know player Coord to these
        monster->go_to.x = p_x;
        monster->go_to.y = p_y;
    }
    else
    {
        if (monster->go_to.x != -1 || monster->go_to.y != -1) //last know Coords are set
            MoveCloserToPlayer(m_x, m_y, monster->go_to.x, monster->go_to.y);
        //move toward last know Coord 
        //or give monster global Coords with a time out if it does not see the player in X number of turns (ESP smart)
        //look if mob can see player and if so update go Coords
        if (DetectPlayer(monster->pos.x, monster->pos.y, &p_x, &p_y)) //use updated pos Coords
        {
            monster->go_to.x = p_x;
            monster->go_to.y = p_y;
        }
    }

    return 1;
}

int MonsterAI::CheckValidMove(int x, int y, int dir)
{
    Coord new_pos;

    switch (dir)
    {
    case dNorth:new_pos.x = x;		new_pos.y = y - 1; break;
    case dEast:	new_pos.x = x + 1;	new_pos.y = y; break;
    case dWest:	new_pos.x = x - 1;	new_pos.y = y; break;
    case dSouth:new_pos.x = x;		new_pos.y = y + 1; break;
    case dNorthEast:	new_pos.x = x + 1;	new_pos.y = y - 1; break;
    case dSouthEast:	new_pos.x = x + 1;	new_pos.y = y + 1;	break;
    case dNorthWest:	new_pos.x = x - 1;	new_pos.y = y - 1;	break;
    case dSouthWest:	new_pos.x = x - 1;	new_pos.y = y + 1; break;
    }
    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    if (dungeonLevel->getCell(new_pos.x, new_pos.y).terrain.type == stone)
        return 0;

    // if cell contains friendly monster return 0
    Monster * mob = dungeonLevel->getCell(new_pos.x, new_pos.y).GetMonster();
    if (mob)
    {
        if (World.getMonsterManager().FindMonsterData(mob)->isPlayer())
        {
            return 1;
        }
        else
            return 0;
    }

    return 1;
}

// determines new Coords to closer postion to player
// move diagonal until mob is equal x or y then move closer
// This emulates mirror movement and eliminates escape potential
// I.e Move in x or y first allows player to out manoeurve AI
int MonsterAI::MoveCloserToPlayer(int m_x, int m_y, int p_x, int p_y)
{
    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    int dir = dWait;

    int dirX = 0;  //-1 left, 1 right
    int dirY = 0;

    dirX = (p_x - m_x) < 0 ? dWest : dEast;
    dirY = (p_y - m_y) < 0 ? dNorth : dSouth;

    if (abs(p_x - m_x) > abs(p_y - m_y) && p_y == m_y) //further away in x direction
    {
        dir = dirX;
        if (!CheckValidMove(m_x, m_y, dirX))  //if dirX is blocked
        {
            if (CheckValidMove(m_x, m_y, dirY + dirX)) //move diagonal
                dir = dirY + dirX;
            else dir = dirX + (dirY == dSouth ? dNorth : dSouth); //if diag blocked move other diagonal
        }
    }
    else if (abs(p_x - m_x) < abs(p_y - m_y) && p_x == m_x) //further away in y direction
    {
        dir = dirY;
        if (!CheckValidMove(m_x, m_y, dirY))  //if dirY is blocked
        {
            if (CheckValidMove(m_x, m_y, dirY + dirX)) //move diagonal
                dir = dirY + dirX;
            else dir = dirY + (dirX == dWest ? dEast : dWest); //if diag blocked move other diagonal
        }
    }
    else //equal so move diagonally
    {
        if (p_y - m_y < 0 && p_x - m_x < 0) //dNorthWest
        {
            dir = dNorthWest;
            if (!CheckValidMove(m_x, m_y, dir))
            {
                if (abs(p_x - m_x) > abs(p_y - m_y))
                {
                    dir = dWest;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorthWest;//force random more			
                }
                else
                {
                    dir = dNorth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dWest;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorthWest;//force random more	
                }
            }
        }
        else if (p_y - m_y < 0 && p_x - m_x > 0) //dNorthEast
        {
            dir = dNorthEast;
            if (!CheckValidMove(m_x, m_y, dir))
            {
                if (abs(p_x - m_x) > abs(p_y - m_y))
                {
                    dir = dEast;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorthEast;//force random more			
                }
                else
                {
                    dir = dNorth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dEast;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dNorthEast;//force random more	
                }


            }

            /*	dir = dNorthEast;
                if(!CheckValidMove(m_x,m_y,dir))
                dir = dNorth;
                if(!CheckValidMove(m_x,m_y,dir))
                dir = dEast;
                if(!CheckValidMove(m_x,m_y,dir))
                dir = dNorthEast;*/
        }
        else if (p_y - m_y > 0 && p_x - m_x < 0) //dSouthWest
        {
            dir = dSouthWest;
            if (!CheckValidMove(m_x, m_y, dir))
            {
                if (abs(p_x - m_x) > abs(p_y - m_y))
                {
                    dir = dWest;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouthWest;//force random more			
                }
                else
                {
                    dir = dSouth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dWest;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouthWest;//force random more	
                }


            }
        }
        else if (p_y - m_y > 0 && p_x - m_x > 0) //dSouthEast
        {
            dir = dSouthEast;
            if (!CheckValidMove(m_x, m_y, dir))
            {
                if (abs(p_x - m_x) > abs(p_y - m_y))
                {
                    dir = dEast;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouthEast;//force random more			
                }
                else
                {
                    dir = dSouth;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dEast;
                    if (!CheckValidMove(m_x, m_y, dir))
                        dir = dSouthEast;//force random more	
                }


            }
        }
    }

    Coord new_pos;

    new_pos.x = m_x;
    new_pos.y = m_y;

    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:	 new_pos.x++; break;
    case dWest:	 new_pos.x--; break;
    case dSouth: new_pos.y++; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    }

    //monsters hate water
    /*	if(World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_pos.x, new_pos.y).terrain.type == deepWater
            && Random::getInt(10,0)) //random chance the mob chases in the water
            {
            if(sqrt( (m_x-p_x)*(m_x-p_x) +   (m_y - p_y)*(m_y - p_y))>1) //attack if close
            {
            RandomMove(current_monster); //find better route to player (random for now)
            return 1;
            }
            }*/

    //dont attack friend
    if ((World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_pos.x, new_pos.y).GetMonster()))
    if (!World.getMonsterManager().FindMonsterData(World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_pos.x, new_pos.y).GetMonster())->isPlayer())
    {
        RandomMove(current_monster); //find better route to player (random for now)
        return 1;
    }

    dungeonLevel->getCell(m_x, m_y).RemoveMonsterRef();
    current_monster->NextAction(World.getActionManager().UpdateAction(&current_monster->action, aMove, new_pos.x, new_pos.y));

    return 1;
}

int MonsterAI::DetectCheese(int m_x, int m_y, int &p_x, int &p_y)
{
    p_x = 0;
    p_y = 0;

    int smell_range = 10;

    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    for (int x = m_x - 10; x < (m_x + 10); x++)
    {
        for (int y = m_y - 10;  y < (m_y + 10); y++)
        {
            // valid cell
            if (x > 0 && y > 0 && x < DUNGEON_SIZE_W - 1 && y < DUNGEON_SIZE_H - 1)
            {
                Item * item = dungeonLevel->getCell(x, y).getItem();
                if (item && item->type == cheese)
                {
                    p_x = x;
                    p_y = y;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int MonsterAI::DetectPlayer(int m_x, int m_y, int *p_x, int *p_y)
{
    int sight;
    if (current_monster->GetState() == asleep)
    {
        sight = current_monster->monster.sight_range / 2 + 1; // current_monster->monster.asleep_sight_Range;
        if (Random::getInt(10, 0) > 1) //20%
            return 0; // did not wake up
    }
    else
        sight = current_monster->monster.sight_range;

    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    /*	//check radius for enemies
        int min_h_range = m_y-sight;
        if(min_h_range < 0)
        min_h_range = 0;

        int min_w_range = m_x-sight;
        if(min_w_range < 0)
        min_w_range = 0;

        int max_h_range = m_y+sight;
        if(max_h_range > DUNGEON_SIZE_H)
        max_h_range = DUNGEON_SIZE_H;

        int max_w_range = m_x+sight;
        if(max_w_range > DUNGEON_SIZE_W)
        max_w_range = DUNGEON_SIZE_W;*/


    //check for enemy - at this stage it just checks player
    //cheat code - passes players position into LOS code and tests if it can be detectec
    MonsterData* player = &(*World.getMonsterManager().Player());

    if (sqrt((float)(m_x - player->pos.x)*(m_x - player->pos.x) + (player->pos.y - m_y)*(player->pos.y - m_y)) <= sight)
    {
        //if(dungeonLevel->IsCellVisible(m_x,m_y,player->pos.x,player->pos.y))
        if (dungeonLevel->IsCellVisible(player->pos.x, player->pos.y, m_x, m_y)) //if the player cant see you then you cant see him
        {
            *p_x = player->pos.x;
            *p_y = player->pos.y;
            return 1;
        }
    }

    //new code - two slow
    /*	for( int h =min_h_range;h<max_h_range;h++)
        {
        for( int w=min_w_range;w<max_w_range;w++)
        {
        int r = sqrt((m_x-w)*(m_x-w) + (h-m_y)*(h-m_y));
        if( r<=sight)
        {
        if(dungeonLevel->IsCellVisible(m_x,m_y,w,h))
        {
        if(dungeonLevel->getCell(w, h).GetMonster())
        {
        if(World.getMonsterManager().FindMonsterData(dungeonLevel->getCell(w, h).GetMonster())->isPlayer())
        {
        *p_x = w;
        *p_y = h;
        return 1;
        }
        }
        }
        }
        }
        }*/

    //old code
    /*	for( int h =min_h_range;h<max_h_range;h++)
        {
        for( int w=min_w_range;w<max_w_range;w++)
        {

        if(LOS(m_x,m_y,w,h,sight))
        {
        if(dungeonLevel->getCell(w, h).GetMonster())
        {
        if(World.getMonsterManager().FindMonsterData(dungeonLevel->getCell(w, h).GetMonster())->isPlayer())
        {
        *p_x = w;
        *p_y = h;
        return 1;
        }
        }
        }
        }
        }*/

    return 0;
}


int MonsterAI::LOS(int m_x, int m_y, int p_x, int p_y, int range)
{
    DungeonLevel* dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    int r = (int)sqrt((float)(m_x - p_x)*(m_x - p_x) + (p_y - m_y)*(p_y - m_y));

    if (r > range) //square is outside radius
        return 0;
    if (p_x == m_x && p_y == m_y)
        return 1;


    if (abs(p_x - m_x) > abs(p_y - m_y))  //further away in x direction
    {
        if (dungeonLevel->getCell(m_x, m_y).terrain.type == stone || dungeonLevel->getCell(m_x, m_y).terrain.type == closedDoor)
            return 0;

        if (p_x - m_x < 0)
        {
            return LOS(m_x - 1, m_y, p_x, p_y, range);
        }
        else
        {
            return LOS(m_x + 1, m_y, p_x, p_y, range);
        }
    }
    else if (abs(p_x - m_x) < abs(p_y - m_y)) //further away in m_y direction
    {
        if (dungeonLevel->getCell(m_x, m_y).terrain.type == stone || dungeonLevel->getCell(m_x, m_y).terrain.type == closedDoor)
            return 0;

        if (p_y - m_y < 0)
        {
            return LOS(m_x, m_y - 1, p_x, p_y, range);
        }
        else
        {
            return LOS(m_x, m_y + 1, p_x, p_y, range);
        }


    }

    else						//equal
    {
        if (dungeonLevel->getCell(m_x, m_y).terrain.type == stone || dungeonLevel->getCell(m_x, m_y).terrain.type == closedDoor)
            return 0;

        if (p_x - m_x < 0 && p_y - m_y < 0)
            return LOS(m_x - 1, m_y - 1, p_x, p_y, range);

        else if (p_x - m_x > 0 && p_y - m_y > 0)
            return LOS(m_x + 1, m_y + 1, p_x, p_y, range);

        else if (p_x - m_x > 0 && p_y - m_y < 0)
            return LOS(m_x + 1, m_y - 1, p_x, p_y, range);

        else if (p_x - m_x < 0 && p_y - m_y > 0)
            return LOS(m_x - 1, m_y + 1, p_x, p_y, range);


    }
    return 1;
}