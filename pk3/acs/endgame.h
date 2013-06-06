#define ENDBASE     (36 * 15)

script ENDGAME_SCOREBOARD (void)
{
    int time = 0;
    int ocl, cl;
    int i;
    int exitStart = 35;
    int endTime;
    int ready, oready;
    int needed;

    SetActivator(0);
    cl = ClientCount();
    SortTimes();

    for (i = 0; i < PLACEMAX; i++)
    {
        if (TimeDisplays[i][2] != -1)
        {
            exitStart += 35;
        }
    }

    endTime = exitStart + ENDBASE;

    while (1)
    {
        oready = ready;
        ready = ReadyExitSum();
        needed = min(RealPlayerCount(), (RealPlayerCount() / 2) + 1);

        if (time == endTime) { break; }
        if (ready >= needed) { break; }

        ocl = cl;
        cl = ClientCount();

        if (time % 35 == 0 || cl > ocl || time == exitStart || oready != ready)
        {
            SetHudSize(320, 240, 1);

            SetFont("BIGFONT");
            HudMessage(s:"End Tally"; HUDMSG_FADEOUT, 2500, CR_WHITE, 160.4, 40.2, 2.0, 1.0);

            SetHudSize(640, 480, 1);

            for (i = 0; i < min(time / 35, PLACENAMECOUNT); i++)
            {
                if (TimeDisplays[i][2] == -1)
                {
                    HudMessage(s:""; HUDMSG_PLAIN, 2600 + (i*4), 0, 0, 0, 0, 0);
                    HudMessage(s:""; HUDMSG_PLAIN, 2601 + (i*4), 0, 0, 0, 0, 0);
                    HudMessage(s:""; HUDMSG_PLAIN, 2602 + (i*4), 0, 0, 0, 0, 0);
                    //HudMessage(s:""; HUDMSG_PLAIN, 2603 + (i*4), 0, 0, 0, 0, 0);
                    continue;
                }

                SetFont("BIGFONT");
                HudMessage(s:GetPlaceName(i); HUDMSG_FADEOUT, 2600 + (i*4), CR_UNTRANSLATED,
                        320.4, 100.0 + (i * 25.0), 2.0, 1.0);
                SetFont("SMALLFONT");

                HudMessage(s:getTime(TimeDisplays[i][1], 1); HUDMSG_FADEOUT, 2601 + (i*4), CR_WHITE,
                        355.1, 100.0 + (i * 25.0), 2.0, 1.0);

                HudMessage(s:getString(TimeDisplays[i][0]); HUDMSG_FADEOUT, 2602 + (i*4), CR_WHITE,
                        285.2, 100.0 + (i * 25.0), 2.0, 1.0);

                //HudMessage(d:TimeDisplays[i][0]; HUDMSG_FADEOUT, 2603 + (i*4), CR_WHITE,
                //        130.0, 100.0 + (i * 30.0), 2.0, 1.0);
            }

            if (time >= exitStart)
            {
                SetFont("SMALLFONT");
                HudMessage(s:"Runners, hit your use key to ready up."; HUDMSG_FADEOUT, 2501, CR_WHITE, 320.0, 400.0, 2.0, 1.0);
                HudMessage(s:"(\cd", d:ready, s:"\c- of \cd", d:needed, s:"\c- needed)"; HUDMSG_FADEOUT, 2502, CR_WHITE, 320.0, 420.0, 2.0, 1.0);
            }
        }

        if (time == exitStart)
        {
            GiveInventory("EnableUseExit", 1);
        }

        time++;
        Delay(1);
    }

    Exit_Normal(0);
}
