#define MODECOUNT   4

#define MODE_NORMAL         0
#define MODE_COUNTDOWN      1
#define MODE_SUDDENDEATH    2
#define MODE_SCOREBOARD     3

#define ENDGAME_OPEN    511
#define ENDGAME_ENTER   512
#define ENDGAME_SWITCH  513

int CRGameMode = MODE_NORMAL;
int CRSwitchTime = 0;
int CRSwitchTo = -1;
int CRSwitchLock;

int CRModeNames[MODECOUNT] =
{
    "Normal",
    "Countdown",
    "Sudden Death",
    "Scoreboard",
};

script ENDGAME_OPEN open
{
    while (1)
    {
        SetHudSize(640, 480, 1);
        HudMessage(s:"Mode: ", s:CRModeNames[CRGameMode];
                HUDMSG_PLAIN, 7741, CR_WHITE, 120.1, 100.0, 1.0, 1.0);

        if (CRSwitchLock)
        {
            HudMessage(s:"Switching to \"", s:CRModeNames[CRSwitchTo], s:"\"... ", d:CRSwitchTime;
                    HUDMSG_PLAIN, 7742, CR_WHITE, 120.1, 120.0, 1.0, 1.0);
        }
        else
        {
            HudMessage(s:"";
                    HUDMSG_PLAIN, 7742, CR_WHITE, 120.1, 120.0, 1.0, 1.0);
        }
        Delay(1);
    }
}

script ENDGAME_ENTER enter
{

}


script ENDGAME_SWITCH (int to1, int wait, int to2)
{
    int i;
    to1--; to2--;

    if (CRSwitchLock) { SetResultValue(0); terminate; }

    CRSwitchLock = 1;
    SetResultValue(1);

    if (to1 > -1)
    {
        CRGameMode = to1;
    }

    if (to2 > -1)
    {
        CRSwitchTo = to2;

        for (i = wait; i > 0; i--)
        {
            CRSwitchTime = i;
            Delay(1);
        }

        CRGameMode = to2;
    }

    CRSwitchTime = 0;
    CRSwitchTo   = -1;
    CRSwitchLock = 0;
}
