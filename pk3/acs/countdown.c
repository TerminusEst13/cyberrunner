#include "zcommon.acs"
#library "countdown"

int FreezeTime = 36*5;

script 901 OPEN
{
    if (!ThingCountName(0, "UseCountdown")) { terminate; }

    while (FreezeTime--)
    {
        if (FreezeTime % 35 == 0)
        {
            SetHudSize(320, 240, 1);
            HudMessageBold(d:FreezeTime/35, s:"..."; HUDMSG_FADEOUT, 121, CR_BRICK,
                    160.4, 140.0, 0.25, 1.0);
        }
        Delay(1);
    }
}


script 902 ENTER
{
    SetPlayerProperty(0, 1, PROP_TOTALLYFROZEN);
}
