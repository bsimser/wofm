#pragma once

#include <string.h>

class MagicScreen
{
public:
    MagicScreen(void);
    virtual ~MagicScreen(void);

    void Display();
    int SelectSpell(bool *keys);

private:
    int CastSpell(bool *keys);
};
