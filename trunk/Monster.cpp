// Monster.cpp: implementation of the Monster class.
//
//////////////////////////////////////////////////////////////////////

#include "Monster.h"
#include "NumberGenerator.h"
#include "WorldBuilder.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::vector<std::string> Monster::descriptions;


Monster::Monster() :
    stamina(10),
    skill(1),
    luck(1),
    sight_range(6),
    asleep_sight_Range(3),
    humanoid(false),
    special(false),
    undead(false),
    experience(10),
    speed(10),
    max_stamina(stamina),
    mLevelVariation(0),
    mLevel(0),
    type(0),
    description(0),
    name("")
{
    setColor(255, 255, 255);
    if (descriptions.size() == 0)
        descriptions.push_back(std::string("This creature dwells deep within the mountain. "));

}

Monster::Monster(const Monster& rhs) :ResistanceBrands(rhs)
{
    name  = rhs.name;
    symbol = rhs.symbol;
    color1 = rhs.color1;
    color2 = rhs.color2;
    color3 = rhs.color3;

    stamina = rhs.stamina;

    skill = rhs.skill;
    luck  = rhs.luck;

    sight_range = rhs.sight_range;
    asleep_sight_Range = rhs.asleep_sight_Range;

    humanoid = rhs.humanoid;
    special = rhs.special;
    undead = rhs.undead;
    
    experience =  rhs.experience;
    max_stamina = rhs.max_stamina;

    speed = rhs.speed;
    type = rhs.type;
    description = rhs.description;
}

Monster::~Monster()
{
}

int Monster::Create(int _type, int level, int level_variation)
{
    mLevel = level;
    mLevelVariation = level_variation;
    type = _type;

    level += getInt(level_variation, 0);

    switch (type)
    {
    case mPlayer:       return createPlayer();          break;
    case mOrc:          return createOrc(level);        break;
    case mWizard:       return createWarlock();         break;
    case mSpecial:      return createSpecial(level);    break;
    case mDigger:       return createDigger();          break;
    case mBridgeMaster: return createBridgeMaster();    break;
    case mCrocodile:     createCrocodile();    break;

    default: throw std::exception("Invalid monster type passed into monster creator"); break;
    }

    return 1;
}

int Monster::dead()
{
    symbol = 'c';
    return 1;
}

void Monster::setColour(unsigned char c1, unsigned char c2, unsigned char c3)
{
    color1 = c1;
    color2 = c2;
    color3 = c3;
}

int Monster::createPlayer()
{
    symbol = '@';
    name = "Player";
    setDescription("Our hero. ");

    color1 = 200;
    color2 = 200;
    color3 = 40;

    skill = 7 + getInt(4, 1); //8-11
    //stamina = getInt(7,1)+ getInt(7,1) + (12-skill); //8 - 18
    stamina = (12 - skill) * 2 + 8 + getInt(3, 1);  //9-18

    if (skill == 8)
        luck = 11 + Random::getInt(2, 0);
    if (skill == 11)
        luck = 7 + Random::getInt(2, 0);
    else 
        luck = 12 - skill + 6 + Random::getInt(3, 0);

    sight_range = 6;
    max_stamina = stamina;

    humanoid = true;

#ifdef _DEBUG
    //skill = 50;
#endif

    return 1;
}

int Monster::createSpecial(int level)
{
    int type = level * 10; //getInt(100,0);

    setColor(getInt(256, 20), getInt(256, 20), getInt(256, 20)); //random colour (Warning: can be black)

    sight_range = getInt(10, 5);

    special = true;

    experience = type / 5;

    if (type < 10) //0
    {
        symbol = 'o';
        name = "orc sentry";
        setDescription("This orc refuses to let you in, he has a key on his belt. ");
        humanoid = true;
        type = 50;
        sight_range = 17; //outside

        stamina = 5;
        skill = 7;
        luck = 3;
    }
    else if (type < 20) //1
    {
        symbol = 's';
        name = "snake";
        setDescription("A small aggressive snake. ");
        skill += 2;
        SetBrand(bPoison, 3);
        stamina = 4;
        skill = 8;
        luck = 3;

        SetResistance(bPoison, 10);
    }
    else if (type < 30) //2
    {
        symbol = 'd';
        name = ("dog");
        setDescription("A large mean dog. ");
        stamina = 8;
        skill = 8;
        luck = 3;
    }
    else if (type < 40) //3
    {
        symbol = 'o';
        humanoid = true;
        name = ("orcish butcher");
        setDescription("This crazed orc is covered head-to-toe in blood and gore and so is its cleaver. ");
        stamina = 8;
        skill = 9;
        luck = 3;
    }
    else if (type < 50)
    {
        symbol = '@';
        name = ("Boathouse Keeper");
        setDescription("This large mean-looking man is calling for his dog. ");
        humanoid = true;
        stamina = 8;
        skill = 10;
        luck = 3;
    }
    else if (type < 60) // 5
    {
        symbol = 'z';
        name = ("skeleton");
        setDescription("A clattering walking skeleton. ");
        humanoid = true;

        stamina = 10;
        skill = 10;
        luck = 3;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 5);
        SetResistance(bFire, -10);
        SetResistance(bLightning, -5);
    }
    else if (type < 70) //6
    {
        symbol = 'C';
        name = ("iron cyclops");
        setDescription("A statue made of iron, but why is its head turning towards you? ");

        stamina = 15;
        skill = 10;
        luck = 3;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 10);
        SetResistance(bLightning, -10);
    }
    else if (type < 80)
    {
        symbol = 'V';
        name = ("master vampire");
        setDescription("A master vampire, are then any other kind? ");
        SetBrand(bConfusion, 2);
        stamina = 11;
        skill = 9;
        luck = 3;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 5);
        SetResistance(bFire, -10);
        SetResistance(bLightning, -5);
    }
    else if (type < 90)
    {
        symbol = 'M';
        name = ("minotaur");
        humanoid = true;
        setDescription("A large bull-man monstrosity. His hooves pound and steam shoots from his nostrils. ");

        stamina = 15;
        skill = 10;
        luck = 3;
    }
    else
    {
        //symbol = 'D';
        //strcpy(name,"Dragon");
        createWarlock();

        return 1;
    }

    max_stamina = stamina;

    return 1;
}

int Monster::createWerewolf()
{
    symbol = 'W';
    name = ("Werewolf");
    setDescription("A large scruffy lythro...err,  lithra, no um, lycanothdo... werewolf. ");

    skill++;
    SetResistance(bPoison, 10);

    stamina += 5;
    skill += 2;
    return 1;
}

void Monster::createCrocodile()
{
    symbol = 'C';
    name = ("crocodile");

    setDescription("A large predatory reptile with long jaws, sharp claws and a hungry smile. ");

    setColor(0, 128, 64);

    stamina = 10;
    skill = 8 + getInt(3, 0);
}

int Monster::createDigger()
{
    symbol = '\\';
    name = ("magical tools");

    setDescription("A collection of digging tools, seemily controlled by invisible hands. ");

    setColor(255, 128, 0);

    SetResistance(bPoison, 10);
    SetResistance(bFrost, 10);
    SetResistance(bFire, 10);

    stamina = 10;
    skill   = 10;
    sight_range = 1;
    return 1;
}
int Monster::createBridgeMaster(bool wereRat)
{
    if (wereRat)
    {
        symbol = 'R';
        name = ("FerryWereRat");
        setDescription("A ferry-ware-rat, and you still don't have the money to pay him! ");

        skill++;
        SetResistance(bPoison, 10);

        stamina = 6 + getInt(7, 1);
        skill+=2;
        luck = 6 + getInt(7, 1);

        sight_range = 5;
        max_stamina = stamina;
    }
    else
    {
        symbol = '@';
        name = ("Ferryman");
        setDescription("A ferryman, too bad you don't have the money to pay him. ");

        setColor(200, 100, 200);

        stamina = 6 + getInt(7, 1);
        skill = 5 + getInt(7, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 5;
        max_stamina = stamina;
    }

    experience = 40;

    humanoid = true;
    return 1;
}


int Monster::createWarlock()
{
    symbol = '@';
    name = ("Zagor");
    setDescription("A regular all-round evil wizard. His hands crackle with unknown magic. ");

    setColor(80, 40, 40);

    stamina = 18;
    skill = 12;
    luck = 6 + getInt(7, 1);

    sight_range = 8;
    max_stamina = stamina;

    experience = 200;

    humanoid = true;

    switch (Random::getInt(4, 1))
    {
    case 0: SetResistance(bPoison, 10); break;
    case 1: SetResistance(bFrost, 10); break;
    case 2: SetResistance(bFire, 10); break;
    case 3: SetResistance(bLightning, 10); break;
    }

    return 1;
}

int Monster::createOrc(int level)
{
    int mLevel = level;
    symbol = 'o';

    color1 = 0;
    color2 = 255;
    color3 = 0;

    humanoid = true;

    if (level > 9)
        mLevel = level / 2;

    static bool chief = false;
    static bool shaman = false;

    int varient = mLevel;//getInt(5,0);

    if (level == 0) //only create orcs;
    {
        name = ("orc");
        setDescription("A basic orc whose only job is to stop people like you getting in the mountain. ");

        color2 = 255;
        sight_range = 18;

        skill = getInt(9, 6);
        stamina = getInt(8, 6);
        luck = 0;

        max_stamina = stamina;

        experience = 20;
        return 1;
    }

    else
        //varient = 
        varient = varient + getInt(7, 1) + getInt(7, 1) - 6;

    if (varient < 2)
    {
        name = ("orc servant"); 	color1 = 255;
        experience = 2;
        setDescription("An orcish servant looking to take out its long suffering on you. ");
    }
    else if (varient < 3)
    {
        name = ("orc");
        setDescription("This orc wanders around the mountain depths causing trouble for people like you. ");
        experience = 4;
    }
    else if (varient < 5)
    {
        name = ("orc archer"); color1 = 255; color2 = 128;
        setDescription("This orc has some quarrelsome friends that it wants you to meet. How nice! ");
        experience = 8;
        varient += 2;
    }
    else if (varient < 6)
    {
        name = ("orc sentry"); color2 = 128;
        setDescription("A slightly better equipped orc than most. He looks like he has been on guard duty a long time. ");
        experience = 6;
    }
    /*	else if(varient <9)
        {
        strcpy(name,"Orc Shaman");
        setColor(220,0,150);
        experience = 8;
        }*/

    else if (varient < 10)
    {
        name = ("orc warrior"); color1 = 237; color2 = 28; color3 = 36;
        setDescription("A mean brute, looking to start some trouble. ");
        experience = 8;
        varient += 2;

    }
    else  if (varient == 10)
    {
        name = ("chieftain's servant"); varient /= 2; color1 = 200;
        setDescription("Looks like he has just been beaten up by someone. ");
        experience = 10;
    }
    else
    {
        name = ("orc chieftain"); varient += 2;	color1 = 0; color2= 128; color3 = 128;
        setDescription("A leader of orcs. Must be tough then. ");
        experience = 20;
        chief = true;
    }

    sight_range = 6;

    stamina = getInt(5, 3) + varient / 2;
    skill   = getInt(5, 3) + varient / 2;
    luck = 0;

    max_stamina = stamina;

    return 1;
}

void Monster::setColor(int c1, int c2, int c3)
{
    color1 = c1;
    color2 = c2;
    color3 = c3;
}

void Monster::setDescription(const std::string & about)
{
    description = 0;
    for (std::vector<std::string>::iterator it = descriptions.begin(); it != descriptions.end(); ++it, ++description)
    {
        if (*it == about)
            return;
    }
    descriptions.push_back(about);
}

const std::string Monster::getDescription() const
{
    return descriptions[description];
}
