#define MODECOUNT   5

#define MODE_NORMAL         1
#define MODE_COUNTDOWN      2
#define MODE_SUDDENDEATH    3
#define MODE_PURGE          4
#define MODE_SCOREBOARD     5

#define MODES_OPEN        511
#define MODES_ENTER       512
#define MODES_DISCONNECT  513
#define MODES_SWITCH      514

#define MODES_OPEN_CLIENT 521
#define MODES_INFORM      522

int CRGameMode = MODE_NORMAL;
int CRSwitchTime = 0;
int CRSwitchTo = -1;
int CRSwitchLock;
int CRSwitched = 0;

int SuddenDeathEnd = 0;

int CRModeNames[MODECOUNT+1] =
{
    "GrossHack",
    "Normal",
    "Countdown",
    "Sudden Death",
    "Failure Purge",
    "Scoreboard",
};

function void CheckCRMusic(void)
{
    switch (CRGameMode)
    {
      case MODE_COUNTDOWN:
        AmbientSound("end/klaxon", 127);
        SetMusic("SILENCE");
        break;

      case MODE_SUDDENDEATH:
        SetMusic("M_HURRY");
        break;

      case MODE_NORMAL:
        SetMusic("*");
        break;

      case MODE_PURGE:
        AmbientSound("ui/lose", 127);
        SetMusic("SILENCE");
        break;
    }
}

script MODES_OPEN open
{
    int clients, oclients;
    while (1)
    {
        oclients = clients;
        clients = ClientCount();

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

        if (clients != oclients || CRSwitched)
        {
            ACS_ExecuteAlways(MODES_INFORM, 0, CRGameMode, CRSwitchTo);
        }

        if (CRGameMode == MODE_SUDDENDEATH)
        {
            if (CRSwitched) { SuddenDeathEnd = Timer() + 2100; }

            if (Timer() == SuddenDeathEnd)
            {
                ACS_ExecuteAlways(MODES_SWITCH, 0, MODE_PURGE, 105, MODE_SCOREBOARD);
            }
        }

        Delay(1);
    }
}

script MODES_ENTER enter
{
    int pln = PlayerNumber();
    int oplace, place;

    place = PlayerPlace[pln];

    while (1)
    {
        oplace = place;
        place = PlayerPlace[pln];

        if (oplace != place && place == 0) // Are we first?
        {
            ACS_ExecuteAlways(MODES_SWITCH, 0, MODE_COUNTDOWN, 70, MODE_SUDDENDEATH);
        }

        Delay(1);
    }
}

script MODES_DISCONNECT (int pln) disconnect
{
    int i;

    for (i = 0; i < PLAYERMAX; i++)
    {
        if (TimeDisplays[i][2] == pln) { RemoveTime(i); }
    }

    DefragTimes();

    if (TimeDisplays[0][0] == -1)  // no more first place
    {
        ACS_ExecuteAlways(MODES_SWITCH, 0, MODE_NORMAL);
    }
}


script MODES_SWITCH (int to1, int wait, int to2)
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

script MODES_OPEN_CLIENT open clientside
{
    while (1)
    {
        if (CRSwitched)
        {
            CheckCRMusic();
        }
        Delay(1);
    }
}

script MODES_INFORM (int gmode, int nmode) clientside
{
    if (IsServer) { terminate; }

    CRGameMode = gmode;
    CRSwitchTo = nmode;

    CRSwitched = 1;
    Delay(1);
    CRSwitched = 0;
}
