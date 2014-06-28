#include ".\magicscreen.h"
#include "WorldBuilder.h"

MagicScreen::MagicScreen(void)
{
}

MagicScreen::~MagicScreen(void)
{
}

int MagicScreen::SelectSpell(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC]) //inventory look
    {
        keys[VK_ESC] = false;
        keys[VC_X] = false;
        return 0;
    }
    else
    {
        return CastSpell(keys);
    }
}

int MagicScreen::CastSpell(bool *keys)
{
    int ret = 1;

    for (int i = 65; i < 89; i++) //a - w
    {
        if (keys[i] == true)
        {
            monsterData* player = WorldBuilder::monsterManager.Player();
            //ret = WorldBuilder::spellManager.CallSpellRoutine(WorldBuilder::monsterManager.Player(),i-65);
            ret = player->NextAction(WorldBuilder::actionManager.UpdateAction(&player->action, aCastSpell, i - 65));

            keys[i] = false;
            break;
        }
    }
    return ret; //return 0 for sucess or 1 for non valid spell. This will keep the screen open
}


void MagicScreen::Display()
{

    WorldBuilder::textManager.ClearDisplayLines();

    monsterData* player = WorldBuilder::monsterManager.Player();

    WorldBuilder::textManager.SetDisplayLine(0, "Spell List");
    WorldBuilder::textManager.SetDisplayLine(1, "==========");



    if (player->spellList.size() == 0)
    {
        WorldBuilder::textManager.SetDisplayLine(3, "You know no spells. ");
    }
    else
    {
        WorldBuilder::textManager.SetDisplayLine(3, "You shuffle your deck, you now have the power of Zagor at your command. ");
        WorldBuilder::textManager.SetDisplayLine(5, "    Spell                Description");
    }

    MONSTERSPELLLIST::iterator sp;

    int start = 7;
    int i = start;

    for (sp = player->spellList.begin(); sp != player->spellList.end(); sp++, i++)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "(%c) %s       %s  ", i + 97 - start, (*sp)->Name(), (*sp)->Desription().c_str());

    }


    WorldBuilder::textManager.SetDisplayLine(39, "[a-d] to select spell, [x] to cancel");

}