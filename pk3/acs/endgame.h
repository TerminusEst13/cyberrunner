#define MODECOUNT   4

#define MODE_NORMAL         1
#define MODE_COUNTDOWN      2
#define MODE_SUDDENDEATH    3
#define MODE_SCOREBOARD     4

#define ENDGAME_OPEN        511
#define ENDGAME_ENTER       512
#define ENDGAME_DISCONNECT  513
#define ENDGAME_SWITCH      514

int CRGameMode = MODE_NORMAL;
int CRSwitchTime = 0;
int CRSwitchTo = -1;
int CRSwitchLock;
int CRSwitched = 0;

int CRModeNames[MODECOUNT+1] =
{
    "GrossHack",
    "Normal",
    "Countdown",
    "Sudden Death",
    "Scoreboard",
};

function void CheckCRMusic(void)
{
    switch (CRGameMode)
    {
      case MODE_COUNTDOWN:
        LocalAmbientSound("end/klaxon", 127);
        LocalSetMusic("SILENCE");
        break;

      case MODE_SUDDENDEATH:
        LocalSetMusic("M_HURRY");
        break;

      case MODE_NORMAL:
        LocalSetMusic("*");
        break;
    }
}

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
    int pln = PlayerNumber();
    int oplace, place;

    place = PlayerPlace[pln];

    CheckCRMusic();

    while (1)
    {
        oplace = place;
        place = PlayerPlace[pln];

        if (oplace != place && place == 0) // Are we first?
        {
            ACS_ExecuteWithResult(ENDGAME_SWITCH, MODE_COUNTDOWN, 70, MODE_SUDDENDEATH);
        }

        if (CRSwitched) { CheckCRMusic(); }

        Delay(1);
    }
}

script ENDGAME_DISCONNECT (int pln) disconnect
{
    int i;

    for (i = 0; i < PLAYERMAX; i++)
    {
        if (TimeDisplays[i][2] == pln) { Print(s:"ded"); RemoveTime(i); }
    }

    DefragTimes();

    if (TimeDisplays[0][0] == -1)  // no more first place
    {
        ACS_ExecuteWithResult(ENDGAME_SWITCH, MODE_NORMAL);
    }
}


script ENDGAME_SWITCH (int to1, int wait, int to2)
{
    int i;
    int start = Timer();
    SetActivator(0);

    CRSwitchLock = start;

    if (to1 > 0)
    {
        CRGameMode = to1;
    }

    if (to2 > 0)
    {
        CRSwitchTo = to2;
        CRSwitched = 1;

        for (i = wait; i > 0; i--)
        {
            if (CRSwitchLock != start) { terminate; }
            CRSwitchTime = i;
            Delay(1);
            CRSwitched = 0;
        }

        CRGameMode = to2;
    }

    CRSwitchTime = 0;
    CRSwitchTo   = -1;
    CRSwitchLock = 0;

    CRSwitched = 1;
    Delay(1);
    CRSwitched = 0;
}
