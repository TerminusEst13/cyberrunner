#include "zcommon.acs"

#library "cyberrun"

function int min(int x, int y)
{
    if (x < y) { return x; }
    return y;
}

function int max (int x, int y)
{
    if (x > y) { return x; }
    return y;
}

function int middle(int x, int y, int z)
{
    if ((x < z) && (y < z)) { return max(x, y); }
    return max(min(x, y), z);
}

function int magnitudeThree_f(int x, int y, int z)
{
    int len, ang;

    ang = VectorAngle(x, y);
    if (((ang + 0.125) % 0.5) > 0.25) { len = FixedDiv(y, sin(ang)); }
    else { len = FixedDiv(x, cos(ang)); }

    ang = VectorAngle(len, z);
    if (((ang + 0.125) % 0.5) > 0.25) { len = FixedDiv(z, sin(ang)); }
    else { len = FixedDiv(len, cos(ang)); }

    return len;
}

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
    int time = 0;

    while (1)
    {
        if (time % 5 == 0)
        {
            TakeInventory("DashCooldown",1);
            TakeInventory("JumpCooldown",1);
            TakeInventory("BoostCooldown",1);
        }
        if (time % 20 == 0) { GiveInventory("PlasmaGunAmmo",1); }
        if (time % 105 == 0) { GiveInventory("ForceVentAmmo", 1); }

        time++;
        delay(1);
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
