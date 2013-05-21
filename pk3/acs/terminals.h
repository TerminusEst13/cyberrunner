#define DISPLAY_NORMAL  0
#define DISPLAY_TITLE   1

world int 21:TerminalMessages[]; 
world int 22:TerminalGraphics[]; 
world int 23:TerminalTitles[]; 
world int 24:TerminalLocation[]; 
world int 25:TerminalNext[];
world int 26:TerminalPrev[];

int TerminalLocks[PLAYERMAX];

function int GetTermDisplayMode(int index)
{
    if (TerminalMessages[index] < 0) { return DISPLAY_TITLE; }
    return DISPLAY_NORMAL;
}

function int GetTermMessage(int index)
{
    int ret = abs(TerminalMessages[index]);
    return ret;
}

function int GetTermGraphic(int index)
{
    int ret = abs(TerminalGraphics[index]);

    if (ret == 0 || ret == "") { return "EMPTYGFX"; }
    return ret;
}

function int GetTermTitle(int index)
{
    int ret = TerminalTitles[index];
    if (ret == 0 || ret == "") { return "Cyberrunner Infoterm"; }
    return ret;
}

function int GetTermLocation(int index)
{
    int ret = TerminalLocation[index];
    if (ret == 0 || ret == "") { return "36E0::1C13::3D15::88A8::1974::13EF::AD9F::DDC4"; }
    return ret;
}

function int GetNextTerm(int index)
{
    int ret = TerminalNext[index];
    return ret;
}

function int GetPrevTerm(int index)
{
    int ret = TerminalPrev[index];
    return ret;
}

script 106 (int indexes, int messageNum, int graphicNum) clientside
{
    int index = leftShort(indexes);
    int next  = rightShort(indexes);

    if (index == 0) { terminate; }

    TerminalMessages[index] = messageNum;
    TerminalGraphics[index] = graphicNum;
    TerminalNext[index]     = next;

    if (next > 0) { TerminalPrev[next] = index; }
}

script 107 (int index, int titleNum, int locationNum) clientside
{
    if (index == 0) { terminate; }

    TerminalTitles[index]    = titleNum;
    TerminalLocation[index] = locationNum;
}

script 108 (int index, int nextScript, int nextDelay)
{
    ACS_ExecuteAlways(110, 0, index);

    if (ConsolePlayerNumber() == -1) { InTerminal[PlayerNumber()] = 1; }

    while (!CheckInventory("TerminalOver")) { Delay(1); }

    if (ConsolePlayerNumber() == -1) { InTerminal[PlayerNumber()] = 0; }
    
    if (nextScript && CheckInventory("TerminalFinished"))
    {
        Delay(nextDelay);
        ACS_ExecuteAlways(nextScript, 0, index);
    }
}


script 109 (int ended) net
{
    if (ended == 1) { GiveInventory("TerminalFinished", 1); }
    if (ended == 2) { UnfreezeDelay[PlayerNumber()] = 3; }

    GiveInventory("TerminalOver", 1);
    Delay(1);
    TakeInventory("TerminalOver", 1);
    TakeInventory("TerminalFinished", 1);
}

script 110 (int startIndex) clientside
{
    int index = startIndex;
    int count, i;
    int oindex;
    int displaymode, message, graphic, next, prev;
    int time = 0;
    int which = 1;
    int urgent;
    int health, ohealth;
    int allowscroll;
    int startPrev = GetPrevTerm(startIndex);
    int backmove;

    if (PlayerNumber() != ConsolePlayerNumber()) { terminate; }

    SetHudSize(1024, 768, 1);

    i = index;

    while (i > 0)
    {
        count++;
        i = GetNextTerm(i);
    }

    health = GetActorProperty(0, APROP_Health);

    while (1)
    {
        if (index <= 0) { urgent = -1; break; }

        ohealth = health;
        health = GetActorProperty(0, APROP_Health);
        InTerminal[PlayerNumber()] = 1;

        if (health < ohealth)
        {
            urgent = 1;
            break;
        }

        displaymode = GetTermDisplayMode(index);
        message     = GetTermMessage(index);
        graphic     = GetTermGraphic(index);
        next        = GetNextTerm(index);
        prev        = GetPrevTerm(index);
        backmove    = 1;

        while (1)
        {
            if (GetTermDisplayMode(prev) == DISPLAY_TITLE &&
                (GetNextTerm(prev) <= 0 || GetPrevTerm(prev) <= 0))
            {
                backmove++;
                prev = GetPrevTerm(prev);
            }
            else
            {
                break;
            }
        }

        if (time % 35 == 0)
        {
            SetHudSize(1024, 768, 1);
            SetFont("TERMBG");
            HudMessage(s:"A"; HUDMSG_FADEOUT, 4000, CR_UNTRANSLATED,
                            512.4, 384.0, 1.5, 1.0);
            SetFont("CONFONT");

            SetHudSize(640, 480, 1);

            switch ((time / 105) % 2)
            {
              case 0:
                HudMessage(s:"Press ", k:"+forward", s:"/", k:"+back", s:" to move forward and back";
                        HUDMSG_FADEOUT, 3001, CR_GREEN, 6.1, 410.0, 1.5, 1.0);
                break;

              case 1:
                HudMessage(s:"Hit ", k:"+jump", s:" to leave the terminal";
                        HUDMSG_FADEOUT, 3001, CR_GREEN, 6.1, 410.0, 1.5, 1.0);
                break;
            }
        }

        if (time % 35 == 0 || oindex != index)
        {
            SetHudSize(640, 480, 1);
            SetFont("CONFONT");
            HudMessage(s:GetTermTitle(index);
                        HUDMSG_FADEOUT, 3003, CR_GREEN, 6.1, 70.0, 1.5, 1.0);

            HudMessage(s:GetTermLocation(index);
                        HUDMSG_FADEOUT, 3004, CR_GREEN, 634.2, 70.0, 1.5, 1.0);

            HudMessage(s:"Screen ", d:which, s:"/", d:count;
                        HUDMSG_FADEOUT, 3002, CR_GREEN, 634.2, 410.0, 1.5, 1.0);

            if (graphic != "EMPTYGFX")
            {
                if (displaymode == DISPLAY_TITLE)
                {
                    SetFont(graphic);
                    HudMessage(s:"A"; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED, 320.4, 240.0, 1.5, 1.0);
                    
                    SetFont("CONFONT");
                    HudMessage(s:message; HUDMSG_FADEOUT, 3005, CR_GREEN, 320.4, 334.1, 1.5, 1.0);
                }
                else
                {
                    SetFont(graphic);
                    HudMessage(s:"A"; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED, 192.4, 240.0, 1.5, 1.0);
                    
                    SetFont("CONFONT");
                    HudMessage(s:message; HUDMSG_FADEOUT, 3005, CR_GREEN, 336.1, 128.1, 1.5, 1.0);
                }
            }
            else
            {
                SetFont("CONFONT");
                HudMessage(s:" "; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED, 320.4, 240.0, 1.5, 1.0);

                if (displaymode == DISPLAY_TITLE)
                {
                    HudMessage(s:message; HUDMSG_FADEOUT, 3005, CR_GREEN, 320.4, 240.0, 1.5, 1.0);
                }
                else
                {
                    HudMessage(s:message; HUDMSG_FADEOUT, 3005, CR_GREEN, 64.1, 128.1, 1.5, 1.0);
                }
            }
        }

        // If we hit a title screen and it's either the first or last
        if (oindex != index && displaymode == DISPLAY_TITLE)
        {
            if (prev <= 0 || next <= 0)
            {
                if (next <= 0) { ActivatorSound("terminal/logout", 127); }
                else if (prev <= 0) { ActivatorSound("terminal/login", 127); }
                 
                i = time;
            }
        }

        oindex = index;

        if (displaymode == DISPLAY_TITLE && (prev <= 0 || next <= 0))
        {
            allowscroll = 0;

            if (i + 70 < time)
            {
                index = next;
                which++;
            }
        }
        else
        {
            allowscroll = 1;
        }

        if (keyPressed(BT_JUMP)) { break; }

        if (allowscroll)
        {
            if (keyPressed(BT_FORWARD))
            {
                index = next;
                which++;
                LocalAmbientSound("terminal/page", 127);
            }

            if (keyPressed(BT_BACK) && prev > 0 && prev != startPrev)
            {
                index = prev;
                which -= backmove;
                LocalAmbientSound("terminal/page", 127);
            }
        }
        
        time++;
        Delay(1);
    }

    SetFont("EMPTYGFX");
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3001, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3002, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3003, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3004, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3005, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);
    HudMessage(s:"A"; HUDMSG_FADEOUT, 4000, CR_UNTRANSLATED,
                    512.4, 384.0, 1.5, 1.0);


    InTerminal[PlayerNumber()] = 0;

    switch (urgent)
    {
      case -1: ConsoleCommand("puke -109 2"); break;
      case 1: ConsoleCommand("puke -109 1"); break;
      default: ConsoleCommand("puke -109"); break;
    }
}

// Super debug script
script 118 ENTER
{
    ACS_ExecuteAlways(106, 0, packShorts(7, 5), -("Test 1 - title with graphic"), "CYBRLOG1");
    ACS_ExecuteAlways(106, 0, packShorts(5, 6), "Test 2 - normal text blob with graphic", "CYBRLOG1");
    ACS_ExecuteAlways(106, 0, packShorts(6, 8), "Test 3 - normal text blob without graphic", "");
    ACS_ExecuteAlways(106, 0, packShorts(8, 9), -("Test 4 - title without graphic"), "");
    ACS_ExecuteAlways(106, 0, packShorts(9, 2), "Test 5 - normal text blob without graphic, after title", "");
    ACS_ExecuteAlways(106, 0, packShorts(2, 0), -("Test 6 - title with graphic, at end"), "CYBRLOG1");

    ACS_ExecuteAlways(107, 0, 6, "Title test", "Location test");
}

script 119 (void)
{
    Print(s:"Sup fags");
}
