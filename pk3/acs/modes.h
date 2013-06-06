#define MODECOUNT   6

#define MODE_NORMAL         1
#define MODE_COUNTDOWN      2
#define MODE_SUDDENDEATH    3
#define MODE_PURGE          4
#define MODE_SCOREBOARD     5
#define MODE_ABORT          6

int CRGameMode = MODE_NORMAL;
int CRSwitchTime = 0;
int CRSwitchTo = -1;
int CRSwitchLock;
int CRSwitched = 0;

int SuddenDeathEnd = 0;

int ClientPlace;

int CRModeNames[MODECOUNT+1] =
{
    "GrossHack",
    "Normal",
    "Countdown",
    "Sudden Death",
    "Failure Purge",
    "Scoreboard",
    "Sudden Death Abort",
};

function void CheckCRMusic(void)
{
    switch (CRGameMode)
    {
      case MODE_COUNTDOWN:
        if (ClientPlace < 1)
        {
            AmbientSound("end/klaxon", 127);
            SetMusic("SILENCE");
        }
        break;

      case MODE_SUDDENDEATH:
        if (ClientPlace < 1)
        {
            SetMusic("M_HURRY");
        }
        break;

      case MODE_NORMAL:
        SetMusic("*");
        break;

      case MODE_PURGE:
        switch (ClientPlace)
        {
          case 1: AmbientSound("ui/winall", 127); break;
          case 0: AmbientSound("ui/lose", 127); break;
          default: AmbientSound("ui/win", 127); break;
        }

        SetMusic("M_SCORES");
        break;

      case MODE_SCOREBOARD:
        break;
    }
}

script MODES_OPEN open
{
    int clients, oclients;
    int inform;

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
            inform = 3;
        }

        if (inform)
        {
            ACS_ExecuteAlways(MODES_INFORM, 0, CRGameMode, CRSwitchTo);
        }

        inform = max(inform-1, 0);

        if (CRGameMode == MODE_SUDDENDEATH)
        {
            if (CRSwitched) { SuddenDeathEnd = Timer() + 210; }

            if (Timer() == SuddenDeathEnd)
            {
                ACS_ExecuteAlways(MODES_SWITCH, 0, MODE_PURGE, 70, MODE_SCOREBOARD);
            }
        }

        if (CRSwitched && CRGameMode == MODE_SCOREBOARD)
        {
            ACS_ExecuteAlways(ENDGAME_SCOREBOARD, 0);
        }

        Delay(1);
    }
}

script MODES_ENTER enter
{
    int pln = PlayerNumber();
    int oplace, place;
    int selfnuke;

    place = PlayerPlace[pln];

    while (1)
    {
        oplace = place;
        place = PlayerPlace[pln];

        if (oplace != place && place == 0) // Are we first?
        {
            ACS_ExecuteAlways(MODES_SWITCH, 0, MODE_COUNTDOWN, 70, MODE_SUDDENDEATH);
        }

        if (CRSwitched && CRGameMode == MODE_PURGE)
        {
            if (place < 0)
            {
                selfnuke = 35;
                SetHudSize(480, 360, 1);
                HudMessage(s:"You have failed."; HUDMSG_FADEOUT, 819, CR_RED, 240.4, 120.0, 1.0, 1.0);
            }
            else if (place == 0)
            {
                SetHudSize(320, 240, 1);
                HudMessage(s:"You win!"; HUDMSG_FADEOUT, 819, CR_GOLD, 160.4, 80.0, 1.0, 1.0);
            }
            else
            {
                SetHudSize(480, 360, 1);
                HudMessage(s:"You pass."; HUDMSG_FADEOUT, 819, CR_WHITE, 240.4, 120.0, 1.0, 1.0);
            }
        }

        if (CRGameMode == MODE_SCOREBOARD)
        {
            SetInventory("HideCRHud", 1);
            SetInventory("ForceParkourOff", 1);

            SetPlayerProperty(0, 1, PROP_TOTALLYFROZEN);
            SetPlayerProperty(0, 1, PROP_FLY);
            SetActorProperty(0, APROP_RenderStyle, STYLE_None);
        }

        if (selfnuke)
        {
            selfnuke = max(0, selfnuke-1);

            if (!selfnuke)
            {
                Spawn("PurgeKiller", GetActorX(0), GetActorY(0), GetActorZ(0) + GetActorViewHeight(0));
            }
        }

        if (CheckInventory("EnableUseExit") && keyDown(BT_USE))
        {
            ReadyToExit[pln] = 1;
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
    int pln  = PlayerNumber();
    int cpln = ConsolePlayerNumber();

    while (1)
    {
        if (PlayerIsSpectator(cpln)) { ClientPlace = -1; }

        if (CRSwitched)
        {
            CheckCRMusic();
        }

        Delay(1);
    }
}

script MODES_ENTER_CLIENT enter clientside
{
    int pln  = PlayerNumber();
    int cpln = ConsolePlayerNumber();
    if (pln != cpln) { terminate; }

    while (1)
    {
        ClientPlace = CheckInventory("CyberrunnerPlace");
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
