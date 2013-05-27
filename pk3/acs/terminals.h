#define DISPLAY_NORMAL  0
#define DISPLAY_TITLE   1

world int 21:TerminalMessages[]; 
world int 22:TerminalGraphics[]; 
world int 23:TerminalTitles[]; 
world int 24:TerminalLocation[]; 
world int 25:TerminalNext[];
world int 26:TerminalPrev[];

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
    SetActorVelocity(0, 0,0,0, 0,0);
    ACS_ExecuteAlways(110, 0, index);

    if (ConsolePlayerNumber() == -1) { InTerminal[PlayerNumber()] = 1; }

    while (!CheckInventory("TerminalOver")) { Delay(1); }

    if (nextScript && CheckInventory("TerminalFinished"))
    {
        Delay(nextDelay);
        ACS_ExecuteAlways(nextScript, 0, index);
    }

    if (IsServer) { InTerminal[PlayerNumber()] = 0; }
}


script 109 (int ended) net
{
    if (ended == 2) { GiveInventory("TerminalFinished", 1); }
    if (ended == 0) { UnfreezeDelay[PlayerNumber()] = 5; }

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
    int backmove, locmode;

    if (PlayerNumber() != ConsolePlayerNumber()) { terminate; }
    if (InTerminal[PlayerNumber()]) { terminate; }

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
        InTerminal[PlayerNumber()] = 1;

        if (index <= 0) { urgent = -1; break; }

        ohealth = health;
        health = GetActorProperty(0, APROP_Health);

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

        // If we hit a title screen and it's either the first or last
        if (oindex != index && displaymode == DISPLAY_TITLE)
        {
            if (prev <= 0 || next <= 0)
            {
                if (next <= 0) { ActivatorSound("terminal/logout", 127); locmode = 2; }
                else if (prev <= 0) { ActivatorSound("terminal/login", 127); locmode = 1;}
                 
                i = time;
            }
        }

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
            SetFont("TERMFONT");

            SetHudSize(640, 480, 1);

            switch ((time / 105) % 2)
            {
              case 0:
                HudMessage(s:"Press \c[TermWhite]", k:"+moveleft", s:"\c-/\c[TermWhite]", k:"+moveright", s:"\c- to move forward and back";
                        HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3001, "TermGreen", 6.1, 410.0, 1.5, 1.0);
                break;

              case 1:
                HudMessage(s:"Hit \c[TermWhite]", k:"+jump", s:"\c- to leave the terminal";
                        HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3001, "TermGreen", 6.1, 410.0, 1.5, 1.0);
                break;
            }
        }

        if (time % 35 == 0 || oindex != index)
        {
            SetHudSize(640, 480, 1);
            SetFont("TERMFONT");
            HudMessage(s:GetTermTitle(index);
                        HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3003, "TermGreen", 6.1, 70.0, 1.5, 1.0);

            if (locmode == 2) { HudMessage(s:"Disconnecting..."; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3004, "TermGreen", 634.2, 70.0, 1.5, 1.0); }
            else if (locmode == 1) { HudMessage(s:"Establishing connection..."; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3004, "TermGreen", 634.2, 70.0, 1.5, 1.0); }
            else { HudMessage(s:GetTermLocation(index); HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3004, "TermGreen", 634.2, 70.0, 1.5, 1.0); }

            HudMessage(s:"Screen ", d:which, s:"/", d:count;
                        HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3002, "TermGreen", 634.2, 410.0, 1.5, 1.0);

            if (graphic != "EMPTYGFX")
            {
                if (displaymode == DISPLAY_TITLE)
                {
                    SetFont(graphic);
                    SetHudSize(800, 600, 1);
                    HudMessage(s:"A"; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED, 400.4, 300.0, 1.5, 1.0);
                    
                    SetHudSize(640, 480, 1);
                    SetFont("TERMFONT");
                    HudMessage(s:message; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3005, "TermDarkGreen", 320.4, 354.1, 1.5, 1.0);
                }
                else
                {
                    SetFont(graphic);
                    SetHudSize(800, 600, 1);
                    HudMessage(s:"A"; HUDMSG_FADEOUT, 3006, CR_UNTRANSLATED, 148.4, 300.0, 1.5, 1.0);
                    
                    SetHudSize(640, 480, 1);
                    SetFont("TERMFONT");
                    HudMessage(s:message; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3005, "TermDarkGreen", 296.1, 112.1, 1.5, 1.0);
                }
            }
            else
            {
                SetFont("TERMFONT");
                HudMessage(s:" "; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3006, CR_UNTRANSLATED, 320.4, 240.0, 1.5, 1.0);

                if (displaymode == DISPLAY_TITLE)
                {
                    HudMessage(s:message; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3005, "TermDarkGreen", 320.4, 240.0, 1.5, 1.0);
                }
                else
                {
                    HudMessage(s:message; HUDMSG_FADEOUT | HUDMSG_COLORSTRING, 3005, "TermDarkGreen", 16.1, 112.1, 1.5, 1.0);
                }
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
                locmode = 0;
            }
        }
        else
        {
            allowscroll = 1;
        }

        if (keyPressed(BT_JUMP)) { break; }

        if (allowscroll)
        {
            if (keyPressed_any(BT_MOVERIGHT | BT_RIGHT))
            {
                index = next;
                which++;
                locmode = 0;
                LocalAmbientSound("terminal/page", 127);
            }

            if (keyPressed_any(BT_MOVELEFT | BT_LEFT) && prev > 0 && prev != startPrev)
            {
                index = prev;
                which -= backmove;
                locmode = 0;
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


    if (!IsServer) { InTerminal[PlayerNumber()] = 0; }

    switch (urgent)
    {
      case -1: ConsoleCommand("puke -109 2"); break;
      case 1: ConsoleCommand("puke -109 1"); break;
      default: ConsoleCommand("puke -109"); break;
    }
}
