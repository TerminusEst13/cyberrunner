#define ENDBASE     (35 * 15)

script ENDGAME_SCOREBOARD (void)
{
    int time = 0;
    int ocl, cl;
    int i;
    int exitStart = 35;
    int endTime;

    SetActivator(0);
    cl = ClientCount();
    DefragTimes();

    for (i = 0; i < PLACEMAX; i++)
    {
        if (TimeDisplays[i][1] != -1)
        {
            exitStart += 35;
        }
    }

    endTime = exitStart + ENDBASE;

    while (1)
    {
        if (time == endTime) { break; }

        ocl = cl;
        cl = ClientCount();
        if (time % 35 == 0 || cl > ocl)
        {
            SetHudSize(320, 240, 1);

            SetFont("BIGFONT");
            HudMessage(s:"End Tally"; HUDMSG_FADEOUT, 2500, CR_WHITE, 160.4, 40.2, 2.0, 1.0);

            SetHudSize(640, 480, 1);

            for (i = 0; i < (time / 35); i++)
            {
                if (TimeDisplays[i][1] == -1) { continue; }

                SetFont("BIGFONT");
                HudMessage(s:GetPlaceName(i); HUDMSG_FADEOUT, 2600 + (i*3), CR_UNTRANSLATED,
                        320.4, 100.0 + (i * 30.0), 2.0, 1.0);
                SetFont("SMALLFONT");

                HudMessage(s:getTime(TimeDisplays[i][1], 1); HUDMSG_FADEOUT, 2601 + (i*3), CR_WHITE,
                        350.1, 100.0 + (i * 30.0), 2.0, 1.0);

                HudMessage(s:getString(TimeDisplays[i][0]); HUDMSG_FADEOUT, 2602 + (i*3), CR_UNTRANSLATED,
                        290.2, 100.0 + (i * 30.0), 2.0, 1.0);
            }

            if (time >= exitStart)
            {
                SetFont("SMALLFONT");
                HudMessage(s:"Hit your use key to ready up."; HUDMSG_FADEOUT, 2501, CR_WHITE, 320.0, 400.0, 2.0, 1.0);
            }
        }

        time++;
        Delay(1);
    }

    Exit_Normal(0);
}
