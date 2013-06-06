function void hudmessageonactor(int tid, int range, str sprite, str text, int id) // By Caligari 87
{
    int dist, ang, vang, pitch, x, y;
    int HUDX = 1280;
    int HUDY = 960;
    int offset = 0;

    if(sprite != -1)
    {
        setfont(sprite);
        text = "A";
        offset = 0.1;
    }

    sethudsize(HUDX, HUDY, 1);
    x = getactorx(tid) - getactorx(0);
    y = getactory(tid) - getactory(0); 

    vang = vectorangle(x,y);
    ang = (vang - GetActorAngle(0) + 1.0) % 1.0;

    if(((vang+0.125)%0.5) > 0.25) dist = fixeddiv(y, sin(vang));
    else dist = fixeddiv(x, cos(vang));

    if ((ang < 0.2 || ang > 0.8) && dist < range)
    {
        pitch = vectorangle(dist, getactorz(tid) - (getactorz(0) + 41.0));
        pitch = (pitch + GetActorPitch(0) + 1.0) % 1.0;

        x = HUDX/2 - ((HUDX/2) * sin(ang) / cos(ang));
        y = HUDY/2 - ((HUDX/2) * sin(pitch) / cos(pitch));

        if(sprite != -1)
            setfont(sprite);
        hudmessage(s:text; HUDMSG_FADEOUT, id, CR_UNTRANSLATED, (x<<16)+offset, ((y<<16)+offset)-32.0, 0.25, 0.5);
    }
    else { hudmessage(s:" "; HUDMSG_PLAIN, id, CR_UNTRANSLATED, 0, 0, 0.1); }
}

function int fracSec(int tics)
{
    int secs   = tics/36;
    int remain = tics%36;
    int frac = itof(remain)/36;
    return itof(secs) + frac;
}

function int getTime(int tics, int showfrac)
{
    int ret = "";

    int secs = tics/36;
    int secs2 = fracSec(tics) % 60.0;
    int hours = secs / 3600;
    int mins  = (secs % 3600) / 60;
    secs %= 60;

    int s_hours = padStringL(StrParam(d:hours), "0", 2);
    int s_minutes = padStringL(StrParam(d:mins), "0", 2);
    int s_secs = padStringL(StrParam(d:secs), "0", 2);
    int s_secs2;

    if (secs2 < 10.0) { s_secs2 = StrParam(s:"0", f:secs2); }
    else { s_secs2 = StrParam(f:secs2); }

    if (showfrac) { ret = StrParam(s:s_hours, s:"\cu:\c-", s:s_minutes, s:"\cu:\c-", s:s_secs2); }
    else { ret = StrParam(s:s_hours, s:"\cu:\c-", s:s_minutes, s:"\cu:\c-", s:s_secs); }

    return ret;
}


function void drawSpeedometer(int speed, int id, int x, int y, int scale, int width)
{
    int i, j, k;

    SetFont("SMALLFONT");
    x = itof(x); y = itof(y);
    
    for (i = 0; i < width; i++)
    {
        j = (itof(i) * scale) / width;

        if (j < speed)
        {
            k = ceil((speed - j) / scale)-1;
            k = max(0, k % BARFONTCOUNT);
            SetFont(BarGraphics[k]);
            HudMessage(s:"A"; HUDMSG_PLAIN, id+i, CR_UNTRANSLATED, (x + (i * 2.0)) + 0.1, y, 0.5, 0.5);
        }
        else
        {
            HudMessage(s:""; HUDMSG_PLAIN, id+i, 0, 0,0, 0);
        }
    }
}

function int AddTime(int pln, int time)
{
    int i, index = -1;
    int nameindex;

    for (i = 0; i < PLACEMAX; i++)
    {
        if (TimeDisplays[i][1] == -1) { index = i; break; }
    }

    if (index == -1) { return -1; }

    nameindex = addString(StrParam(n:pln+1));

    TimeDisplays[index][0] = nameindex;
    TimeDisplays[index][1] = time;
    TimeDisplays[index][2] = pln;

    return index;
}

function void RemoveTime(int index)
{
    if (TimeDisplays[index][0] >= 0) { freeString(TimeDisplays[index][0]); }
    TimeDisplays[index][0] = -1;
    TimeDisplays[index][1] = -1;
    TimeDisplays[index][2] = -1;
}

function int DefragTimes(void)
{
    int moveup, last, i;

    for (i = 0; i < PLACEMAX; i++)
    {
        if (TimeDisplays[i][1] == -1)
        {
            moveup++;
        }
        else if (moveup > 0)
        {
            TimeDisplays[i-moveup][0] = TimeDisplays[i][0];
            TimeDisplays[i-moveup][1] = TimeDisplays[i][1];
            TimeDisplays[i-moveup][2] = TimeDisplays[i][2];
            RemoveTime(i);
        }
    }

    return PLACEMAX - moveup;  // the amount of actual entries
}

function int GetPlaceName(int place)
{
    if (place >= 0 && place < PLACENAMECOUNT)
    {
        return PlaceNames[place];
    }

    return StrParam(s:"\cu", d:place+1, s:"th");
}

function void SetCheckpoint(int pln, int x, int y, int z, int ang, int pitch, int index, int next)
{
    int i;
    for (i = 0; i < 8; i++) { OldCheckpointCoords[pln][i] = CheckpointCoords[pln][i]; }

    CheckpointCoords[pln][0] = x;
    CheckpointCoords[pln][1] = y;
    CheckpointCoords[pln][2] = z;
    CheckpointCoords[pln][3] = ang;
    CheckpointCoords[pln][4] = pitch;
    CheckpointCoords[pln][5] = index;
    CheckpointCoords[pln][6] = next;
    CheckpointCoords[pln][7] = Timer();
}

function void RevertCheckpoint(int pln) // can only undo once
{
    int i;
    for (i = 0; i < 8; i++) { CheckpointCoords[pln][i] = OldCheckpointCoords[pln][i]; }
}

function int ReadyExitSum(void)
{
    int ret, i;
    for (i = 0; i < PLAYERMAX; i++) { ret += !!ReadyToExit[i]; }
    return ret;
}
