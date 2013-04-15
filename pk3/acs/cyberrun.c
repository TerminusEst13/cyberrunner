#include "zcommon.acs"
#include "commonFuncs.h"

#library "cyberrun"

#define RECHARGECOUNT 8
#libdefine TID_PLAY 600

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
		hudmessage(s:text; HUDMSG_PLAIN, id, CR_UNTRANSLATED, (x<<16)+offset, ((y<<16)+offset)-32.0, 0.1);
	}
	else
		hudmessage(s:" "; HUDMSG_PLAIN, id, CR_UNTRANSLATED, 0, 0, 0.1);
}

int RechargingItems[RECHARGECOUNT] = 
{
    "DashCooldown", "JumpCooldown", "BoostCooldown", "PlasmaGunAmmo",
    "ForceVentAmmo", "CannotIntoShotgun", "CannotIntoCarbine",
    "CannotIntoVulcan",
};

int RechargingTimes[RECHARGECOUNT][2] = 
{
    {5, -1}, {5, -1}, {5, -1}, {15, 1}, {210, 1}, {1, -1}, {1, -1}, {1, -1},
};


// 0, 1, 2  -> x, y, z
// 3, 4     -> angle, pitch
// 5        -> last checkpoint
// 6        -> next checkpoint
int CheckpointCoords[PLAYERMAX][7];

script 105 (int mode, int index, int next)
{
    int pln = PlayerNumber();
    int ttid = unusedTID(10000, 15000);
    int x,y,z;

    //Log(s:"(", d:mode, s:", ", d:index, s:", ", d:next, s:")");

    switch (mode)
    {
      case 0:
        x = CheckpointCoords[pln][0];
        y = CheckpointCoords[pln][1];
        z = CheckpointCoords[pln][2];
        Spawn("ACSSpawnDummy", x,y,z, ttid, CheckpointCoords[pln][3]);
        Spawn("CyberTelefog", x,y,z);
        Spawn("CyberTelefog", GetActorX(0), GetActorY(0), GetActorZ(0));

        Teleport_NoFog(ttid);
        
        SetActorAngle(0, CheckpointCoords[pln][3]);
        SetActorPitch(0, CheckpointCoords[pln][4]);
        SetActorVelocity(0, 0,0,0, 0,0);
        
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
        CheckpointCoords[pln][5] = index;
        CheckpointCoords[pln][6] = next;

        if (mode % 2)
        {
            SetHudSize(320, 240, 0);
            LocalAmbientSound("ui/checkpoint", 127);
            HudMessage(s:"Checkpoint!"; HUDMSG_FADEOUT, 701, CR_YELLOW,
                        160.4, 60.0, 2.0, 0.5);
        }

        while (!onGround(0)) { Delay(1); }

        CheckpointCoords[pln][0] = GetActorX(0);
        CheckpointCoords[pln][1] = GetActorY(0);
        CheckpointCoords[pln][2] = GetActorZ(0);
        CheckpointCoords[pln][3] = GetActorAngle(0);
        CheckpointCoords[pln][4] = GetActorPitch(0);
    }
}

script 405 OPEN
{
    if (!GetCVar("cyber_noaircontrol"))
    {
        ConsoleCommand("set cyber_noaircontrol 0");
        ConsoleCommand("archivecvar cyber_noaircontrol");
    }
	
    if (!GetCVar("cyber_particles"))
    {
        ConsoleCommand("set cyber_particles 0");
        ConsoleCommand("archivecvar cyber_particles");
    }

    while (1)
    {
        if (!GetCVar("cyber_noaircontrol")) { SetAirControl(0.2); }
        else { SetAirControl(0.00390625); }
        Delay(35);
    }
}

script 400 ENTER
{
    int i, j, time = 0;

    while (1)
    {
        for (i = 0; i < RECHARGECOUNT; i++)
        {
            if (time % RechargingTimes[i][0] == 0)
            {
                j = RechargingTimes[i][1];

                if (j > 0) { GiveInventory(RechargingItems[i], j); }
                else { TakeInventory(RechargingItems[i], -j); }
            }
        }

        time++;

        Delay(1);
    }
}

script 401 DEATH
{
    ACS_Terminate(402, 0);
    ACS_Terminate(403, 0);
    ACS_Terminate(404, 0);
    ACS_Terminate(400, 0);
	TakeInventory("CannotIntoShotgun",1);
	TakeInventory("CannotIntoCarbine",1);
	TakeInventory("CannotIntoVulcan",1);
}

#define DASH_VEL 45

script 402 (void) NET
{
    int vx, vy, vz,  mag, angle, pitch;
    int nx, ny, nz, nmag;
    int rx, ry, rz, rmag;
	
    if (isDead(0)) { terminate; }
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
    if (isDead(0)) { terminate; }
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
    if (isDead(0)) { terminate; }
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
    int t, i;
    int x, y, z;
    int vx, vy, vz, mag, magI;

    x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + random(22.0, 26.0);
    x += random(-2.0, 2.0); y += random(-2.0, 2.0);

    vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
    vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
    magI = ftoi(mag);

    for (i = 8; i < magI; i += 8)
    {
        Spawn("ShotgunTracer", x+(vx*i), y+(vy*i), z+(vz*i));
        if (i % 128 == 0) { Delay(1); }
    }
}

// Hoorah for only three script arguments!
script 414 (int tx, int ty, int tz) clientside
{
    int t, i;
    int x, y, z;
    int vx, vy, vz, mag, magI;

    x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + 24.0;

    vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
    vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
    magI = ftoi(mag);

    for (i = 8; i < magI; i += 8)
    {
        Spawn("RailBeam2", x+(vx*i), y+(vy*i), z+(vz*i));
        if (i % 128 == 0) { Delay(1); }
    }
}

#define RAINBOWCOLORS 12
#define RAINBOWINTERVAL 7

int RainbowBeamActors[RAINBOWCOLORS] = 
{
    "RainbowBeam1",
    "RainbowBeam2",
    "RainbowBeam3",
    "RainbowBeam4",
    "RainbowBeam5",
    "RainbowBeam6",
    "RainbowBeam7",
    "RainbowBeam8",
    "RainbowBeam9",
    "RainbowBeam10",
    "RainbowBeam11",
    "RainbowBeam12",
};

script 415 (int tx, int ty, int tz) clientside
{
    int t, i, j, actor;
    int x, y, z;
    int vx, vy, vz, mag, magI;

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
        if (i % 128 == 0) { Delay(1); }
    }
}

script 416 (int respawning)
{
    int i;

    if (!respawning)
    {
        ACS_ExecuteWithResult(105, 4);
    }
	Thing_ChangeTID(0,TID_PLAY+PlayerNumber());

	TakeInventory("CannotIntoShotgun",1);
	TakeInventory("CannotIntoVulcan",1);
	TakeInventory("CannotIntoCarbine",1);
	TakeInventory("CannotIntoWallhack",1);
	
    if (GetCvar("instagib") == 1)
    {
        GiveInventory("Instagib Rifle",1);
        TakeInventory("Plasma Gun",1);
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

Script 421 (int radar)
{
    switch (radar)
    {
      case 1:
        Delay(1);
        if(CheckActorInventory(ActivatorTID(), "WallHackVision"))
        {
            //for(int m = 0; m < 32; m++)
            //for(int m = 0; m < 64; m++)
            //{
                //HudMessageOnActor(TID_PLAY+m,32000.0,"PLAYMARK","",3515+m);
				if(CheckActorInventory(TID_PLAY+0,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+0,32000.0,"PLAYMARK","",3515+0);}
				if(CheckActorInventory(TID_PLAY+1,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+1,32000.0,"PLAYMARK","",3515+1);}
				if(CheckActorInventory(TID_PLAY+2,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+2,32000.0,"PLAYMARK","",3515+2);}
				if(CheckActorInventory(TID_PLAY+3,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+3,32000.0,"PLAYMARK","",3515+3);}
				if(CheckActorInventory(TID_PLAY+4,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+4,32000.0,"PLAYMARK","",3515+4);}
				if(CheckActorInventory(TID_PLAY+5,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+5,32000.0,"PLAYMARK","",3515+5);}
				if(CheckActorInventory(TID_PLAY+6,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+6,32000.0,"PLAYMARK","",3515+6);}
				if(CheckActorInventory(TID_PLAY+7,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+7,32000.0,"PLAYMARK","",3515+7);}
				if(CheckActorInventory(TID_PLAY+8,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+8,32000.0,"PLAYMARK","",3515+8);}
				if(CheckActorInventory(TID_PLAY+9,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+9,32000.0,"PLAYMARK","",3515+9);}
				if(CheckActorInventory(TID_PLAY+10,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+10,32000.0,"PLAYMARK","",3515+10);}
				if(CheckActorInventory(TID_PLAY+11,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+11,32000.0,"PLAYMARK","",3515+11);}
				if(CheckActorInventory(TID_PLAY+12,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+12,32000.0,"PLAYMARK","",3515+12);}
				if(CheckActorInventory(TID_PLAY+13,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+13,32000.0,"PLAYMARK","",3515+13);}
				if(CheckActorInventory(TID_PLAY+14,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+14,32000.0,"PLAYMARK","",3515+14);}
				if(CheckActorInventory(TID_PLAY+15,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+15,32000.0,"PLAYMARK","",3515+15);}
				if(CheckActorInventory(TID_PLAY+16,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+16,32000.0,"PLAYMARK","",3515+16);}
				if(CheckActorInventory(TID_PLAY+17,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+17,32000.0,"PLAYMARK","",3515+17);}
				if(CheckActorInventory(TID_PLAY+18,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+18,32000.0,"PLAYMARK","",3515+18);}
				if(CheckActorInventory(TID_PLAY+19,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+19,32000.0,"PLAYMARK","",3515+19);}
				if(CheckActorInventory(TID_PLAY+20,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+20,32000.0,"PLAYMARK","",3515+20);}
				if(CheckActorInventory(TID_PLAY+21,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+21,32000.0,"PLAYMARK","",3515+21);}
				if(CheckActorInventory(TID_PLAY+22,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+22,32000.0,"PLAYMARK","",3515+22);}
				if(CheckActorInventory(TID_PLAY+23,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+23,32000.0,"PLAYMARK","",3515+23);}
				if(CheckActorInventory(TID_PLAY+24,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+24,32000.0,"PLAYMARK","",3515+24);}
				if(CheckActorInventory(TID_PLAY+25,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+25,32000.0,"PLAYMARK","",3515+25);}
				if(CheckActorInventory(TID_PLAY+26,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+26,32000.0,"PLAYMARK","",3515+26);}
				if(CheckActorInventory(TID_PLAY+27,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+27,32000.0,"PLAYMARK","",3515+27);}
				if(CheckActorInventory(TID_PLAY+28,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+28,32000.0,"PLAYMARK","",3515+28);}
				if(CheckActorInventory(TID_PLAY+29,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+29,32000.0,"PLAYMARK","",3515+29);}
				if(CheckActorInventory(TID_PLAY+30,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+30,32000.0,"PLAYMARK","",3515+30);}
				if(CheckActorInventory(TID_PLAY+31,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+31,32000.0,"PLAYMARK","",3515+31);}
				if(CheckActorInventory(TID_PLAY+32,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+32,32000.0,"PLAYMARK","",3515+32);}
				if(CheckActorInventory(TID_PLAY+33,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+33,32000.0,"PLAYMARK","",3515+33);}
				if(CheckActorInventory(TID_PLAY+34,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+34,32000.0,"PLAYMARK","",3515+34);}
				if(CheckActorInventory(TID_PLAY+35,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+35,32000.0,"PLAYMARK","",3515+35);}
				if(CheckActorInventory(TID_PLAY+36,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+36,32000.0,"PLAYMARK","",3515+36);}
				if(CheckActorInventory(TID_PLAY+37,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+37,32000.0,"PLAYMARK","",3515+37);}
				if(CheckActorInventory(TID_PLAY+38,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+38,32000.0,"PLAYMARK","",3515+38);}
				if(CheckActorInventory(TID_PLAY+39,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+39,32000.0,"PLAYMARK","",3515+39);}
				if(CheckActorInventory(TID_PLAY+40,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+40,32000.0,"PLAYMARK","",3515+40);}
				if(CheckActorInventory(TID_PLAY+41,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+41,32000.0,"PLAYMARK","",3515+41);}
				if(CheckActorInventory(TID_PLAY+42,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+42,32000.0,"PLAYMARK","",3515+42);}
				if(CheckActorInventory(TID_PLAY+43,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+43,32000.0,"PLAYMARK","",3515+43);}
				if(CheckActorInventory(TID_PLAY+44,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+44,32000.0,"PLAYMARK","",3515+44);}
				if(CheckActorInventory(TID_PLAY+45,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+45,32000.0,"PLAYMARK","",3515+45);}
				if(CheckActorInventory(TID_PLAY+46,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+46,32000.0,"PLAYMARK","",3515+46);}
				if(CheckActorInventory(TID_PLAY+47,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+47,32000.0,"PLAYMARK","",3515+47);}
				if(CheckActorInventory(TID_PLAY+48,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+48,32000.0,"PLAYMARK","",3515+48);}
				if(CheckActorInventory(TID_PLAY+49,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+49,32000.0,"PLAYMARK","",3515+49);}
				if(CheckActorInventory(TID_PLAY+50,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+50,32000.0,"PLAYMARK","",3515+50);}
				if(CheckActorInventory(TID_PLAY+51,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+51,32000.0,"PLAYMARK","",3515+51);}
				if(CheckActorInventory(TID_PLAY+52,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+52,32000.0,"PLAYMARK","",3515+52);}
				if(CheckActorInventory(TID_PLAY+53,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+53,32000.0,"PLAYMARK","",3515+53);}
				if(CheckActorInventory(TID_PLAY+54,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+54,32000.0,"PLAYMARK","",3515+54);}
				if(CheckActorInventory(TID_PLAY+55,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+55,32000.0,"PLAYMARK","",3515+55);}
				if(CheckActorInventory(TID_PLAY+56,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+56,32000.0,"PLAYMARK","",3515+56);}
				if(CheckActorInventory(TID_PLAY+57,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+57,32000.0,"PLAYMARK","",3515+57);}
				if(CheckActorInventory(TID_PLAY+58,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+58,32000.0,"PLAYMARK","",3515+58);}
				if(CheckActorInventory(TID_PLAY+59,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+59,32000.0,"PLAYMARK","",3515+59);}
				if(CheckActorInventory(TID_PLAY+60,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+60,32000.0,"PLAYMARK","",3515+60);}
				if(CheckActorInventory(TID_PLAY+61,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+61,32000.0,"PLAYMARK","",3515+61);}
				if(CheckActorInventory(TID_PLAY+62,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+62,32000.0,"PLAYMARK","",3515+62);}
				if(CheckActorInventory(TID_PLAY+63,"CyberrunnerClass")) {HudMessageOnActor(TID_PLAY+63,32000.0,"PLAYMARK","",3515+63);}
                restart;
            //}
        }
        else
        {
            terminate;
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

#define PARTICLECOUNT 7
#define PARTDIST_MIN  8.0
#define PARTDIST_MAX  32.0
 
int ParticleTypes[PARTICLECOUNT] = {"CyberShotgunSparkle", "CyberCarbineSparkle", "CyberVulcanSparkle", "ForceVentSparkle", "TurboSparkle", "AllAmmoSparkle", "50HPSparkle"};
 
script 422 (int which) clientside
{
    int particle, angle, dist, xmod, ymod, zmod, newtid;
    which = middle(0, which, PARTICLECOUNT-1);
    particle = ParticleTypes[which];
     
    while (1)
    {
        if (GetCVar("cyber_particles") > 0)
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
