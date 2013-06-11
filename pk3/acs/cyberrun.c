#include "zcommon.acs"
#library "cyberrun"

#include "commonFuncs.h"
#include "stralloc.h"

#include "cyberdefs.h"
#include "cyberfuncs.h"

#include "terminals.h"
#include "termDefs.h"
#include "modes.h"
#include "endgame.h"

script 105 (int mode, int index, int next)
{
    int pln = PlayerNumber();
    int ttid = unusedTID(10000, 15000);
    int x,y,z;
    int i, place = -1;

    //Log(s:"(", d:mode, s:", ", d:index, s:", ", d:next, s:")");

    switch (mode)
    {
      case 0:
        if (CheckpointCoords[pln][7] >= Timer() - 1)
        {
            RevertCheckpoint(pln);
        }

        x = CheckpointCoords[pln][0];
        y = CheckpointCoords[pln][1];
        z = CheckpointCoords[pln][2];

        if (!HasTeleported[pln])
        {
            Spawn("CyberTelefog", x,y,z);
            Spawn("CyberTelefog", GetActorX(0), GetActorY(0), GetActorZ(0));
            HasTeleported[pln] = 2;
        }

        Teleport_NoFog(ttid);
        
        SetActorAngle(0, CheckpointCoords[pln][3]);
        SetActorPitch(0, CheckpointCoords[pln][4]);
        SetActorVelocity(0, FixedMul(1.0, cos(GetActorAngle(0))), FixedMul(1.0, sin(GetActorAngle(0))),0.05, 0,0);
        SetActorPosition(0, x,y,z, 0);
        
        SetPlayerProperty(0, 1, PROP_FROZEN);
        Delay(8);
        SetPlayerProperty(0, 0, PROP_FROZEN);
        break;
      
      case 1:
      case 2:
        if (index <= CheckpointCoords[pln][5] && index != CheckpointCoords[pln][6])
            { break; }

      case 3:
      case 4:
        if (mode % 2)
        {
            LocalAmbientSound("ui/checkpoint", 127);

            SetHudSize(320, 240, 1);
            HudMessage(s:"Checkpoint!"; HUDMSG_FADEOUT, 701, CR_YELLOW,
                        160.4, 60.0, 2.0, 0.5);

            SetHudSize(640, 480, 1);

            i = getTime(Timer() - PlayerTimes[pln][TIME_CHECKPOINT], 1);
            HudMessage(s:"\cjTime: \c-", s:i; HUDMSG_FADEOUT, 702, CR_GOLD,
                        250.1, 140.0, 2.0, 0.5);

            i = getTime(Timer() - PlayerTimes[pln][TIME_START], 1);
            HudMessage(s:"\cjTotal: \c-", s:i; HUDMSG_FADEOUT, 703, CR_LIGHTBLUE,
                        246.1, 160.0, 2.0, 0.5);

            PlayerTimes[pln][TIME_CHECKPOINT] = Timer();
        }


        // This is so that a checkpoint can't be *completely* wasted;
        //   at least, not as easily.
        
        SetCheckpoint(pln, GetActorX(0), GetActorY(0), GetActorZ(0), GetActorAngle(0), GetActorPitch(0), index, next);

        while (!onGround(0)) { Delay(1); }

        SetCheckpoint(pln, GetActorX(0), GetActorY(0), GetActorZ(0), GetActorAngle(0), GetActorPitch(0), index, next);
        break;
      
      case 5:
        if (index <= CheckpointCoords[pln][5] && index != CheckpointCoords[pln][6])
            { break; }

      case 6:
        if (PlayerTimes[pln][TIME_FINISH] == 0x7FFFFFFF)
        {
            PlayerTimes[pln][TIME_FINISH] = Timer();
            AddTime(pln, Timer() - PlayerTimes[pln][TIME_START]);
            SortTimes();
            place = FindBestTime(pln);

            if (PlayerPlace[pln] == -1) { PlayerPlace[pln] = place; }
            SetInventory("CyberrunnerPlace", PlayerPlace[pln]+1);
        }

        if (mode % 2)
        {
            LocalAmbientSound("ui/finish", 127);

            SetHudSize(320, 240, 1);
            SetFont("BIGFONT");
            HudMessage(s:"FINISH!"; HUDMSG_FADEOUT, 701, CR_GOLD,
                        160.4, 50.0, 2.0, 0.5);

            SetHudSize(640, 480, 1);
            SetFont("SMALLFONT");

            i = getTime(Timer() - PlayerTimes[pln][TIME_START], 1);
            HudMessage(s:"\cjTime: \c-", s:i; HUDMSG_FADEOUT, 702, CR_LIGHTBLUE,
                        320.4, 140.0, 2.0, 0.5);

            if (place != -1)
            {
                SetHudSize(480, 360, 1);
                HudMessage(s:"Place: \cf", s:GetPlaceName(place); HUDMSG_FADEOUT, 703, CR_BRICK,
                            240.4, 128.0, 2.0, 0.5);
            }


            PlayerTimes[pln][TIME_CHECKPOINT] = Timer();
        }

        if (CRGameMode == MODE_SUDDENDEATH || CRGameMode == MODE_COUNTDOWN) { LocalSetMusic("*"); }


        SetCheckpoint(pln, GetActorX(0), GetActorY(0), GetActorZ(0), GetActorAngle(0), GetActorPitch(0), index, next);

        while (!onGround(0)) { Delay(1); }

        SetCheckpoint(pln, GetActorX(0), GetActorY(0), GetActorZ(0), GetActorAngle(0), GetActorPitch(0), index, next);
    }
}

script 405 OPEN
{
    int ac, oac, i;

    IsServer = 1;

    if (!GetCVar("cyber_noaircontrol"))
    {
        ConsoleCommand("set cyber_noaircontrol 0");
        ConsoleCommand("archivecvar cyber_noaircontrol");
    }

    if (!GetCVar("cyber_superstay"))
    {
        ConsoleCommand("set cyber_superstay 0");
        ConsoleCommand("archivecvar cyber_superstay");
    }

    if (!GetCVar("cyber_runmod"))
    {
        ConsoleCommand("set cyber_runmod 0");
        ConsoleCommand("archivecvar cyber_runmod");
    }

    for (i = 0; i < PLACEMAX; i++)
    {
        RemoveTime(i);
    }

    while (1)
    {
        oac = ac;
        ac = GetCVar("cyber_noaircontrol");
        if (!ac && (oac || (Timer() % 70 == 0))) { SetAirControl(0.2); }
        else if (!oac && ac) { SetAirControl(0.00390625); }
        Delay(1);
    }
}

script 400 ENTER
{
    int i, j, time = 0;
    int x, y, z;
    int tid;
    int pln = PlayerNumber();
    int term, oterm, unfreeze;
    int obegin;
    int place = -1, oplace;

    ACS_ExecuteAlways(424, 0, pln, Timer());
    SetPlayerProperty(0, 0, PROP_TOTALLYFROZEN);
    SetPlayerProperty(0, 0, PROP_FLY);
    SetActorProperty(0, APROP_RenderStyle, STYLE_Normal);
    TakeInventory("HideCRHud", 1);
    TakeInventory("EnableUseExit", 1);
    TakeInventory("ForceParkourOff", 1);

    PlayerTimes[pln][TIME_START]        = Timer();
    PlayerTimes[pln][TIME_CHECKPOINT]   = Timer();
    PlayerTimes[pln][TIME_END]          = 0x7FFFFFFF;
    PlayerPlace[pln]                    = -1;
    Purged[pln]                         = 0;

    while (1)
    {
        tid = defaultTID(-1);
        PlayerTIDs[pln] = tid;
        HasTeleported[pln] = max(0, HasTeleported[pln]-1);

        oplace = place;
        place = PlayerPlace[pln];

        SetInventory("CyberrunnerPlace", PlayerPlace[pln]+1);

        for (i = 0; i < RECHARGECOUNT; i++)
        {
            if (time % RechargingTimes[i][0] == 0)
            {
                j = RechargingTimes[i][1];

                if (j > 0) { GiveInventory(RechargingItems[i], j); }
                else { TakeInventory(RechargingItems[i], -j); }
            }
        }

        if (keyDown(BT_FORWARD | BT_BACK))
        {
            x = GetActorVelX(0);
            y = GetActorVelY(0);
            z = GetActorVelZ(0);

            x = FixedMul(x, 0.8);
            y = FixedMul(y, 0.8);
            if (z < 0) { z = FixedMul(z, 0.95); }

            SetActorVelocity(0, x,y,z, 0,0);
        }

        SetInventory("LeftMouseTapped", keyPressed(BT_ATTACK));
        SetInventory("LeftMouseDown", keyDown(BT_ATTACK));
        SetInventory("RightMouseTapped", keyPressed(BT_ALTATTACK));
        SetInventory("RightMouseDown", keyDown(BT_ALTATTACK));

        oterm = term;
        term = InTerminal[pln];

        if (term && !oterm)
        {
            SetPlayerProperty(0, 1, PROP_TOTALLYFROZEN);
            GiveInventory("ForceParkourOff", 1);
        }

        if (!term && oterm || unfreeze)
        {
            if (UnfreezeDelay[pln]) { unfreeze = 1; }
            else
            {
                SetPlayerProperty(0, 0, PROP_TOTALLYFROZEN);
                TakeInventory("ForceParkourOff", 1);
                unfreeze = 0;
            }
        }

        if (Purged[pln])
        {
            SetPlayerProperty(0, 1, PROP_TOTALLYFROZEN);
            SetActorProperty(0, APROP_RenderStyle, STYLE_None);
        }

        UnfreezeDelay[pln] = max(0, UnfreezeDelay[pln]-1);
        time++;
		
		//SetActorProperty(0,APROP_SPEED,GetCVar("cyber_runmod")); // Well, shit, that would've made things easy.
		
		/*if (GetCVar("cyber_runmod") != 0)
		{
			if (GetCVar("cyber_runmod") == -10) {	SetActorProperty(0,APROP_SPEED,0.05);	}
			if (GetCVar("cyber_runmod") == -9)	{	SetActorProperty(0,APROP_SPEED,0.1);	}
			if (GetCVar("cyber_runmod") == -8)	{	SetActorProperty(0,APROP_SPEED,0.2);	}
			if (GetCVar("cyber_runmod") == -7)	{	SetActorProperty(0,APROP_SPEED,0.3);	}
			if (GetCVar("cyber_runmod") == -6)	{	SetActorProperty(0,APROP_SPEED,0.4);	}
			if (GetCVar("cyber_runmod") == -5)	{	SetActorProperty(0,APROP_SPEED,0.5);	}
			if (GetCVar("cyber_runmod") == -4)	{	SetActorProperty(0,APROP_SPEED,0.6);	}
			if (GetCVar("cyber_runmod") == -3)	{	SetActorProperty(0,APROP_SPEED,0.7);	}
			if (GetCVar("cyber_runmod") == -2)	{	SetActorProperty(0,APROP_SPEED,0.8);	}
			if (GetCVar("cyber_runmod") == -1)	{	SetActorProperty(0,APROP_SPEED,0.9);	}
			if (GetCVar("cyber_runmod") == 1)	{	SetActorProperty(0,APROP_SPEED,1.1);	}
			if (GetCVar("cyber_runmod") == 2)	{	SetActorProperty(0,APROP_SPEED,1.2);	}
			if (GetCVar("cyber_runmod") == 3)	{	SetActorProperty(0,APROP_SPEED,1.3);	}
			if (GetCVar("cyber_runmod") == 4)	{	SetActorProperty(0,APROP_SPEED,1.4);	}
			if (GetCVar("cyber_runmod") == 5)	{	SetActorProperty(0,APROP_SPEED,1.5);	}
			if (GetCVar("cyber_runmod") == 6)	{	SetActorProperty(0,APROP_SPEED,1.6);	}
			if (GetCVar("cyber_runmod") == 7)	{	SetActorProperty(0,APROP_SPEED,1.7);	}
			if (GetCVar("cyber_runmod") == 8)	{	SetActorProperty(0,APROP_SPEED,1.8);	}
			if (GetCVar("cyber_runmod") == 9)	{	SetActorProperty(0,APROP_SPEED,1.9);	}
			if (GetCVar("cyber_runmod") == 10)	{	SetActorProperty(0,APROP_SPEED,2);		}
		}
		else
		{
			SetActorProperty(0,APROP_SPEED,1);
		}*/ // Are you KIDDING ME that didn't work EITHER
		
		if (GetCVar("cyber_runmod") == -10)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-10",1);
			}
		if (GetCVar("cyber_runmod") == -9)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-9",1);
			}
		if (GetCVar("cyber_runmod") == -8)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-8",1);
			}
		if (GetCVar("cyber_runmod") == -7)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-7",1);
			}
		if (GetCVar("cyber_runmod") == -6)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-6",1);
			}
		if (GetCVar("cyber_runmod") == -5)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-5",1);
			}
		if (GetCVar("cyber_runmod") == -4)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-4",1);
			}
		if (GetCVar("cyber_runmod") == -3)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-3",1);
			}
		if (GetCVar("cyber_runmod") == -2)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-2",1);
			}
		if (GetCVar("cyber_runmod") == -1)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod-1",1);
			}
		if (GetCVar("cyber_runmod") == 0)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			}
		if (GetCVar("cyber_runmod") == 1)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod1",1);
			}
		if (GetCVar("cyber_runmod") == 2)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod2",1);
			}
		if (GetCVar("cyber_runmod") == 3)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod3",1);
			}
		if (GetCVar("cyber_runmod") == 4)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod4",1);
			}
		if (GetCVar("cyber_runmod") == 5)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod5",1);
			}
		if (GetCVar("cyber_runmod") == 6)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod6",1);
			}
		if (GetCVar("cyber_runmod") == 7)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod7",1);
			}
		if (GetCVar("cyber_runmod") == 8)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod8",1);
			}
		if (GetCVar("cyber_runmod") == 9)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod9",1);
			}
		if (GetCVar("cyber_runmod") == 10)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod10",1);
			}
		if (GetCVar("cyber_runmod") == 11)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod11",1);
			}
		if (GetCVar("cyber_runmod") == 12)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod12",1);
			}
		if (GetCVar("cyber_runmod") == 13)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod13",1);
			}
		if (GetCVar("cyber_runmod") == 14)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod14",1);
			}
		if (GetCVar("cyber_runmod") == 15)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod15",1);
			}
		if (GetCVar("cyber_runmod") == 16)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod16",1);
			}
		if (GetCVar("cyber_runmod") == 17)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod17",1);
			}
		if (GetCVar("cyber_runmod") == 18)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod18",1);
			}
		if (GetCVar("cyber_runmod") == 19)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod19",1);
			}
		if (GetCVar("cyber_runmod") == 20)
			{
			GiveInventory("PlayerSpeedRemoval",1);
			GiveInventory("CyberRunMod20",1);
			}
		
		

        Delay(1);
    }
}

script 401 DEATH
{
    ACS_Terminate(402, 0);
    ACS_Terminate(403, 0);
    ACS_Terminate(404, 0);
    ACS_Terminate(400, 0);
    TakeInventory("CannotIntoShotgun",999);
    TakeInventory("CannotIntoCarbine",999);
    TakeInventory("CannotIntoVulcan",999);
    TakeInventory("CannotIntoSuper",999);
}

script 402 (void) NET
{
    int vx, vy, vz,  mag, angle, pitch;
    int nx, ny, nz, nmag;
    int rx, ry, rz, rmag;
    
    if (isDead(0) || PlayerIsSpectator(PlayerNumber())) { terminate; }
    if (InTerminal[PlayerNumber()]) { terminate; }
    if (CheckInventory("DashCooldown") == 0)
    {
        vx = GetActorVelX(0);
        vy = GetActorVelY(0);
        vz = GetActorVelZ(0);
        angle = GetActorAngle(0);
        pitch = middle(-0.027, GetActorPitch(0), 0.25);
        mag = magnitudeThree_f(vx, vy, vz);

        nx = FixedMul(DASH_VEL * cos(angle), cos(pitch));
        ny = FixedMul(DASH_VEL * sin(angle), cos(pitch));
        nz = DASH_VEL * -sin(pitch);
        nmag = magnitudeThree_f(nx, ny, nz);
        
        rx = vx + nx; ry = vy + ny; rz = vz + nz;
        rmag = magnitudeThree_f(rx, ry, rz);

        ActivatorSound("cyber/dash",255);

        if (rmag < mag)  // if we're making a sharp turn
        {
            SetActorVelocity(0, nx, ny, nz, 0,0);
        }
        else
        {
            mag += (DASH_VEL<<16);

            SetActorVelocity(0,
                    FixedMul(FixedMul(mag, cos(angle)), cos(pitch)),
                    FixedMul(FixedMul(mag, sin(angle)), cos(pitch)),
                    FixedMul(mag, -sin(pitch)), 0, 0);
        }

        GiveInventory("DashCooldown",30);
    }
}

script 403 (void) NET
{
    if (isDead(0) || PlayerIsSpectator(PlayerNumber())) { terminate; }
    if (InTerminal[PlayerNumber()]) { terminate; }
    if (CheckInventory("JumpCooldown") == 0)
    {
        ActivatorSound("cyber/jump",255);
        ThrustThingZ(0,80,0,0);
        ThrustThing(GetActorAngle(0)/256,12,1,0);
        GiveInventory("JumpCooldown",30);
    }
}

script 404 (void) NET
{
    if (isDead(0) || PlayerIsSpectator(PlayerNumber())) { terminate; }
    if (InTerminal[PlayerNumber()]) { terminate; }
    if (CheckInventory("BoostCooldown") == 0)
    {
        ActivatorSound("cyber/boost",255);
        GiveInventory("CyberBoostJump",1);
        GiveInventory("CyberBoostSpeed",1);
        GiveInventory("BoostCooldown",300);
    }
}


script 412 (int which)
{
    int x, y, z;

    x = GetActorX(0); y = GetActorY(0); z = GetActorZ(0);
    SetActivatorToTarget(0);

    switch (which)
    {
      case 0:
        ACS_ExecuteAlways(413, 0, x, y, z);
        break;

      case 1:
        ACS_ExecuteAlways(414, 0, x, y, z);
        break;

      case 2:
        ACS_ExecuteAlways(415, 0, x, y, z);
        break;
    }
}

script 413 (int tx, int ty, int tz) clientside
{
    int t, i, k = 0, l;
    int x, y, z;
    int vx, vy, vz, mag, magI;

    if (GetCVar("cyber_cl_notrails")) { terminate; }

    x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + random(22.0, 26.0);
    x += random(-2.0, 2.0); y += random(-2.0, 2.0);

    vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
    vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
    magI = ftoi(mag);

    for (i = 8; i < magI; i += 8)
    {
        Spawn("ShotgunTracer", x+(vx*i), y+(vy*i), z+(vz*i));
        
        l += (i - k);
        Delay(l / 512);
        l %= 512;
        k = i;
    }
}

// Hoorah for only three script arguments!
script 414 (int tx, int ty, int tz) clientside
{
    int t, i, k = 0, l;
    int x, y, z;
    int vx, vy, vz, mag, magI;

    if (GetCVar("cyber_cl_notrails")) { terminate; }

    x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + 24.0;

    vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
    vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
    magI = ftoi(mag);

    for (i = 8; i < magI; i += 8)
    {
        Spawn("RailBeam2", x+(vx*i), y+(vy*i), z+(vz*i));
        
        l += (i - k);
        Delay(l / 512);
        l %= 512;
        k = i;
    }
}

script 415 (int tx, int ty, int tz) clientside
{
    int t, i, j, k = 0, l, actor;
    int x, y, z;
    int vx, vy, vz, mag, magI;

    if (GetCVar("cyber_cl_notrails")) { terminate; }

    x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + 24.0;

    vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
    vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
    magI = ftoi(mag);

    j = 0;
    for (i = 8; i < magI; i += 8)
    {
        actor = RainbowBeamActors[(j / RAINBOWINTERVAL) % RAINBOWCOLORS];
        Spawn(actor, x+(vx*i), y+(vy*i), z+(vz*i));
        j++;

        l += (i - k);
        Delay(l / 512);
        l %= 512;
        k = i;
    }
}

script 416 (int respawning)
{
    int i;

    if (!respawning) { ACS_ExecuteWithResult(105, 4); }
    else { ACS_ExecuteWithResult(105, 0); }

    TakeInventory("CannotIntoShotgun",999);
    TakeInventory("CannotIntoVulcan",999);
    TakeInventory("CannotIntoCarbine",999);
    TakeInventory("CannotIntoWallhack",1);
    
    if (GetCvar("instagib") == 1)
    {
        GiveInventory(" Purge Rifle ", 1);
        TakeInventory("Plasma Gun", 1);
        GiveInventory("No-Damage Plasma Gun", 1);
    }
    else if (isLMS())
    {
        GiveInventory(" Cyber Shotgun ",1);
        GiveInventory(" Cyber Carbine ",1);
        GiveInventory(" Cyber Vulcan ",1);
        GiveInventory("CarbineAmmo2",1000);
        GiveInventory("ShotgunAmmo2",1000);
        GiveInventory("VulcanAmmo2",1000);
    }
}

script 417 ENTER   { ACS_ExecuteWIthResult(416); }
script 419 RESPAWN { ACS_ExecuteWIthResult(416, 1); }

script 418 (void) { SetResultValue(!!GetCVar("sv_weaponstay")); }

script 421 (int which)
{
    int i, time, tid;
    int pln = PlayerNumber();
    
    switch (which)
    {
      case 1:
        GiveInventory("WallHackVision",1);
        GiveInventory("CannotIntoWallhack", 1);

        while (1)
        {
            for (i = 0; i < PLAYERMAX; i++)
            {
                if (i == pln || !PlayerInGame(i)) { continue; }
                tid = PlayerTIDs[i];
                if (isDead(tid)) { continue; }

                HudMessageOnActor(tid, 0x7FFFFFFF, "PLAYMARK", "A", tid);
            }

            Delay(5);

            if (!CheckInventory("WallhackVision")) { break; }
            TakeInventory("CannotIntoWallhack", 1);

            Delay(5);
            if (!CheckInventory("WallhackVision")) { break; }
        }
        break;
    
      case 2:
        TakeInventory("WallHackVision",1);
        GiveInventory("CannotIntoWallhack",1);
        Delay(15);
        TakeInventory("CannotIntoWallhack",1);
        break;
    }
}

script 420 DEATH
{
    Thing_ChangeTID(0,0);
}
 
script 422 (int which) clientside
{
    int particle, angle, dist, xmod, ymod, zmod, newtid;
    which = middle(0, which, PARTICLECOUNT-1);
    particle = ParticleTypes[which];
     
    while (1)
    {
        if (GetCVar("cyber_cl_particles"))
        {
            angle = random(0, 1.0);
            dist  = random(PARTDIST_MIN, PARTDIST_MAX);
            xmod  = FixedMul(cos(angle), dist);
            ymod  = FixedMul(sin(angle), dist);
            zmod  = random(8.0, 32.0);
            newtid = unusedTID(14000, 17000);
 
            Spawn(particle, GetActorX(0) + xmod, GetActorY(0) + ymod, GetActorZ(0) + zmod, newtid);
            SetActorVelocity(newtid, 0,0,1.0, 0,0);
        }
        Delay(2);
    }
}

int TimerOn;
script 423 (void) net clientside
{
    TimerOn = !TimerOn;
    int pln = PlayerNumber();
    int time = -1, time2;
    int i,j,k,l;
    int dist, unitCm;
    int myTID;

    if (pln != ConsolePlayerNumber()) { terminate; }

    myTID = defaultTID(-1);
    SetActivator(-1);

    while (TimerOn && !PlayerIsSpectator(pln))
    {
        time++;
        SetHudSize(480, 360, 1);

        i = getTime(time, 1);
        HudMessage(s:i; HUDMSG_FADEOUT, 2501, CR_GOLD, 390.1, 32.0, 1.0, 1.0);

        dist += ftoi(magnitudeThree_f(GetActorVelX(myTID), GetActorVelY(myTID), GetActorVelZ(myTID)));

        Delay(1);
    }

    if (time > 0)
    {
        unitCm = FixedDiv(itof(GetCVar("cyber_mph_doomguyheight")) / 51, 1.2);
        unitCm = FixedMul(unitCm, 2.54);

        time2 = time/36;
        i = time2/3600;
        j = (time2%3600)/60;
        k = fracSec(time);
        l = dist * (unitCm / 100);
        PrintBold(s:"End time: \ck", d:i, s:"\c- hour",   s:cond(i == 1, "", "s"), s:", \ck",
                                 d:j, s:"\c- minute", s:cond(j == 1, "", "s"), s:", \ck",
                                 f:k, s:"\c- second", s:cond(k == 1.0, "", "s"),
              s:"\nDistance travelled: \ck", f:l, s:"\c- meter", s:cond(l == 1.0, "", "s"));
    }
}

script 424 (int pln, int startTime) clientside
{
    int time;
    int i, f, x, y, z, m, mph, unitCm;
    int showmag = 0;


    if (GetCVar("cyber_cl_debug"))
    {
        int classify = ClassifyActor(0);
        int classifyStr;

        if (classify & ACTOR_WORLD) { classifyStr = "Activator is world"; }
        else if (classify & ACTOR_PLAYER) { classifyStr = "Activator is a player"; }
        else if (classify & ACTOR_MONSTER) { classifyStr = "Activator is a monster"; }
        else { classifyStr = "Actor is... something"; }
        PrintBold(s:"Script started on tic ", d:startTime, s:"\n",
                s:"Player ", d:pln, s:" (the supposed activator) is ", s:cond(PlayerInGame(pln), "\cd", "\chnot "), s:"in game.\c-\n",
                s:"Current activator of script 424 for player ", d:pln, s:" (", n:pln+1, s:"\c-) is \"", n:0, s:"\c-\"\c-\n",
                s:"Activator TID is ", d:ActivatorTID(), s:"\c-\n",
                s:classifyStr,
                s:"\nActivator supposedly at (", f:GetActorX(0), s:", ", f:GetActorY(0), s:", ", f:GetActorZ(0), s:")");
    }

    if (ClassifyActor(0) & ACTOR_WORLD) { terminate; }


    while (1)
    {
        if (GetCVar("screenblocks") >= 12 || InTerminal[pln] || CheckInventory("HideCRHud"))
        {
            for (x = 0; x < 10; x++)
            {
                HudMessage(s:""; HUDMSG_PLAIN, 1500+x, 0, 0,0, 0);
            }

            HudMessage(s:""; HUDMSG_PLAIN, 3500, 0, 0,0, 0);

            for (x = 0; x < 50; x++)
            {
                HudMessage(s:""; HUDMSG_PLAIN, 3501+x, 0, 0,0, 0);
            }

            time++;
            showmag = 0;

            Delay(1);
            continue;
        }

        if (GetCVar("cyber_cl_timer") > 0)
        {
            SetHudSize(480, 360, 0);

            i = getTime(time, 0);
            SetFont("SMALLFONT");
            HudMessage(s:i; HUDMSG_FADEOUT, 1500, CR_WHITE, 390.1, 20.0, 1.0, 1.0);
        }

        x = GetActorVelX(0);
        y = GetActorVelY(0);

        if (GetCVar("cyber_mph_noz")) { z = 0; }
        else { z = GetActorVelZ(0); }

        m = magnitudeThree_f(x, y, z);

        f = GetCVar("cyber_mph_friction");

        if (f <= 100) { showmag = m; }
        else if (abs(m - showmag) < (900.0 / f)) { showmag = m; }
        else
        {
            showmag += FixedDiv(m - showmag, itof(f) / 100);
        }

        unitCm = FixedDiv(itof(GetCVar("cyber_mph_doomguyheight")) / 51, 1.2);
        unitCm = FixedMul(unitCm, 2.54);

        mph = FixedMul(m, unitCm) / 100;   // meters/tic
        mph = FixedMul(mph, SECOND_TICS);   // meters/second

        if (GetCVar("cyber_mph_km")) { mph /= 1000; } // km/second  
        else { mph = FixedDiv(mph, 1609.344); } // miles/second
        mph *= 3600;                        // mph
        mph >>= 16;

        SetHudSize(640, 480, 1);
        SetFont("SMALLFONT");

        if (GetCvar("cyber_cl_debug") >= 2 && Timer() % 36 == 0)
        {
            Print(s:"\nTime is ", d:time, s:"\n",
                s:"Activator name is ", n:0, s:"\c- (TID ", d:ActivatorTID(), s:")\n",
                s:"MPH: (", f:x, s:", ", f:y, s:", ", f:z, s:")\nmag is ", f:m, s:" (on bar: ", f:showmag, s:")\n",
                s:"Mode is ", d:GetCVar("cyber_mph_km"), s:", mph is ", d:mph);
        }

        switch (GetCVar("cyber_mph_km"))
        {
          case 3:
            HudMessage(f:m, s:"\c- units/tic"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5);
            break;

          case 2:
            HudMessage(d:round(m), s:"\c- units/tic"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5);
            break;

          case 1:
            HudMessage(d:mph, s:"\c- km/h"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5);
            break;

          case 0:
            HudMessage(d:mph, s:"\c- mph"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5);
            break;
        }

        SetHudSize(800, 600, 1);

        drawSpeedometer(showmag, 3501, 700, 80, GetCvar("cyber_mph_barscale"), 50);

        time++;

        Delay(1);
    }
}

script 425 OPEN clientside
{
    if (!GetCVar("cyber_cl_debug"))
    {
        ConsoleCommand("set cyber_cl_debug 0");
        // No archivecvar on purpose
    }

    if (!GetCVar("cyber_cl_timer"))
    {
        ConsoleCommand("set cyber_cl_timer 0");
        ConsoleCommand("archivecvar cyber_cl_timer");
    }

    if (!GetCVar("cyber_cl_particles"))
    {
        ConsoleCommand("set cyber_cl_particles 0");
        ConsoleCommand("archivecvar cyber_cl_particles");
    }

    if (!GetCVar("cyber_cl_notrails"))
    {
        ConsoleCommand("set cyber_cl_notrails 0");
        ConsoleCommand("archivecvar cyber_cl_notrails");
    }

    if (!GetCVar("cyber_mph_noz"))
    {
        ConsoleCommand("set cyber_mph_noz 0");
        ConsoleCommand("archivecvar cyber_mph_noz");
    }

    if (!GetCVar("cyber_mph_km"))
    {
        ConsoleCommand("set cyber_mph_km 0");
        ConsoleCommand("archivecvar cyber_mph_km");
    }

    if (!GetCVar("cyber_mph_doomguyheight"))
    {
        ConsoleCommand("set cyber_mph_doomguyheight 72");
        ConsoleCommand("archivecvar cyber_mph_doomguyheight");
    }

    if (!GetCVar("cyber_mph_barscale"))
    {
        ConsoleCommand("set cyber_mph_barscale 40");
        ConsoleCommand("archivecvar cyber_mph_barscale");
    }

    if (!GetCVar("cyber_mph_friction"))
    {
        ConsoleCommand("set cyber_mph_friction 300");
        ConsoleCommand("archivecvar cyber_mph_friction");
    }
}

script 426 (int pln) disconnect
{
    InTerminal[pln] = 0;
}

script 427 (int snum, int a1, int a2)
{
    SetActivatorToTarget(0);

    ACS_ExecuteAlways(snum, 0, a1, a2, 0);
}

script 428 (void) { SetResultValue(!!GetCVar("cyber_superstay")); }