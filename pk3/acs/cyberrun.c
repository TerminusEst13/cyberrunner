#include "zcommon.acs"
#include "commonFuncs.h"

#library "cyberrun"

#define RECHARGECOUNT 6

int RechargingItems[RECHARGECOUNT][3] =
{
    {"DashCooldown",    5,      -1},
    {"JumpCooldown",    5,      -1},
    {"BoostCooldown",   5,      -1},
    {"PlasmaGunAmmo",   20,      1},
    {"ForceVentAmmo",   105,     1},
    {"ShotgunAmmo",     70,      1},
};

script 405 OPEN
{
    if (!GetCVar("cyberrunner_noaircontrol"))
    {
        ConsoleCommand("set cyberrunner_noaircontrol 0");
        ConsoleCommand("archivecvar cyberrunner_noaircontrol");
    }

    while (1)
    {
        if (GetCVar("sv_aircontrol") != 0.15 && !GetCVar("cyberrunner_noaircontrol"))
        {
            ConsoleCommand("sv_aircontrol 0.15");
        }
        Delay(1);
    }
}

script 400 ENTER
{
    int i, j, time = 0;

    while (1)
    {
        for (i = 0; i < RECHARGECOUNT; i++)
        {
            if (time % RechargingItems[i][1] == 0)
            {
                j = RechargingItems[i][2];
                
                if (j >= 0) { GiveInventory(RechargingItems[i][0], j); }
                else { TakeInventory(RechargingItems[i][0], -j); }
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
}

#define DASH_VEL 45

script 402 (void) NET
{
    int vx, vy, vz,  mag, angle, pitch;
    int nx, ny, nz, nmag;
    int rx, ry, rz, rmag;

    if (CheckInventory("DashCooldown") == 0)
    {
        vx = GetActorVelX(0);
        vy = GetActorVelY(0);
        vz = GetActorVelZ(0);
        angle = GetActorAngle(0);
        pitch = middle(-0.027, GetActorPitch(0), 0.05);
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
    int t, i;
    int x, y, z, tx, ty, tz;
    int vx, vy, vz, mag, magI;

    switch (which)
    {
      case 0:
        Thing_ChangeTID(0, unusedTID(15000, 25000));
        break;

      case 1:
        SetResultValue(ActivatorTID());
        break;

      case 2:
        t = CheckInventory("ShotgunTracerTID");
        TakeInventory("ShotgunTracerTID", 0x7FFFFFFF);

        x  = GetActorX(0); y =  GetActorY(0);  z = GetActorZ(0) + random(22.0, 26.0);
        x += random(-2.0, 2.0); y += random(-2.0, 2.0);

        tx = GetActorX(t); ty = GetActorY(t); tz = GetActorZ(t);

        vx = tx-x; vy = ty-y; vz = tz-z; mag = magnitudeThree_f(vx, vy, vz);
        vx = FixedDiv(vx, mag); vy = FixedDiv(vy, mag); vz = FixedDiv(vz, mag);
        magI = ftoi(mag);

        for (i = 8; i < magI; i += 8)
        {
            Spawn("ShotgunTracer", x+(vx*i), y+(vy*i), z+(vz*i));
            if (i % 128 == 0) { Delay(1); }
        }
        break;
    }
}
