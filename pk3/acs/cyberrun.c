#include "zcommon.acs"
#include "commonFuncs.h"

#library "cyberrun"

#define RECHARGECOUNT 8

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

	TakeInventory("CannotIntoShotgun",1);
	TakeInventory("CannotIntoVulcan",1);
	TakeInventory("CannotIntoCarbine",1);
	
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
