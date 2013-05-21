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

script 108 (int startIndex) clientside
{
    int index = startIndex;
    int oindex;
    int displaymode, message, graphic, next, prev;
    int time = 0;

    SetHudSize(1024, 768, 1);

    while (1)
    {
        if (index <= 0) { break; }

        displaymode = GetTermDisplayMode(index);
        message     = GetTermMessage(index);
        graphic     = GetTermGraphic(index);
        next        = GetNextTerm(index);
        prev        = GetPrevTerm(index);

        if (time % 35 == 0)
        {
            SetHudSize(1024, 768, 1);
            SetFont("TERMBG");
            HudMessage(s:"A"; HUDMSG_FADEOUT, 4000, CR_UNTRANSLATED,
                            512.4, 384.0, 1.5, 1.0);
            SetFont("CONFONT");

            SetHudSize(640, 480, 1);
            HudMessage(s:"Press ", k:"+forward", s:"/", k:"+back", s:" to move up and down";
                        HUDMSG_FADEOUT, 3001, CR_GREEN, 6.1, 410.0, 1.5, 1.0);
        }

        if (time % 35 == 0 || oindex != index)
        {
            SetHudSize(640, 480, 1);
            SetFont("CONFONT");
            HudMessage(s:GetTermTitle(index);
                        HUDMSG_FADEOUT, 3003, CR_GREEN, 6.1, 70.0, 1.5, 1.0);

            HudMessage(s:GetTermLocation(index);
                        HUDMSG_FADEOUT, 3004, CR_GREEN, 634.2, 70.0, 1.5, 1.0);

            if (displaymode == DISPLAY_TITLE)
            {
                SetFont(graphic);
                HudMessage(s:"A"; HUDMSG_FADEOUT, 3005, CR_UNTRANSLATED, 320.4, 240.0, 1.5, 1.0);
                
                SetFont("CONFONT");
                if (GetChar(message, 0) == '$')
                {
                    HudMessage(l:message; HUDMSG_FADEOUT, 3006, CR_GREEN, 320.4, 366.1, 1.5, 1.0);
                }
                else
                {
                    HudMessage(s:message; HUDMSG_FADEOUT, 3006, CR_GREEN, 320.4, 366.1, 1.5, 1.0);
                }
            }
            else
            {
                SetFont(graphic);
                HudMessage(s:"A"; HUDMSG_FADEOUT, 3005, CR_UNTRANSLATED, 192.4, 240.0, 1.5, 1.0);
                
                SetFont("CONFONT");
                if (GetChar(message, 0) == '$')
                {
                    HudMessage(l:message; HUDMSG_FADEOUT, 3006, CR_GREEN, 336.1, 128.1, 1.5, 1.0);
                }
                else
                {
                    HudMessage(s:message; HUDMSG_FADEOUT, 3006, CR_GREEN, 336.1, 128.1, 1.5, 1.0);
                }
            }
        }

        //Print(d:index, s:" (", d:displaymode, s:"): ", s:message,
        //    s:"\n", s:graphic, 
        //    s:"\n", d:next);

        oindex = index;

        if ((time+1) % 70 == 0)
        {
            index = next;
        }
        
        time++;
        Delay(1);
    }

    SetFont("EMPTYGFX");
    HudMessage(s:"A"; HUDMSG_FADEOUT, 3001, CR_UNTRANSLATED,
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
}

// Super debug script
script 118 ENTER
{
    int derp = "Test 3";
    ACS_ExecuteAlways(106, 0, packShorts(5, 6), "Test 1", "CYBRLOG1");
    ACS_ExecuteAlways(106, 0, packShorts(6, 7), "Test 2", "");
    ACS_ExecuteAlways(106, 0, packShorts(7, 0), -derp, "CYBRLOG1");
}
