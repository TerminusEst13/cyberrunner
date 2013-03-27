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

script 400 ENTER
{
    while (1)
    {
        if (Timer() % 5 == 0)
        {
            TakeInventory("DashCooldown",1);
            TakeInventory("JumpCooldown",1);
            TakeInventory("BoostCooldown",1);
        }
        if (Timer() % 20 == 0) { GiveInventory("PlasmaGunAmmo",1); }

        delay(1);
    }
}

script 405 ENTER
{
	while (TRUE)
	{
        GiveInventory("ForceVentAmmo",1);
        delay(105);
	}
}

script 401 DEATH
{
    ACS_Terminate(402, 0);
    ACS_Terminate(403, 0);
    ACS_Terminate(404, 0);
    ACS_Terminate(400, 0);
}

script 402 (void) NET
{
    int vx, vy, vz, mag, angle, pitch;

    if (CheckInventory("DashCooldown") == 0)
    {
        vx = GetActorVelX(0);
        vy = GetActorVelY(0);
        vz = GetActorVelZ(0);
        angle = GetActorAngle(0);
        pitch = middle(-0.05, GetActorPitch(0), 0.05);
        mag = magnitudeThree_f(vx, vy, vz);
        mag += 42.0;

        ActivatorSound("cyber/dash",255);
        SetActorVelocity(0,
                FixedMul(FixedMul(mag, cos(angle)), cos(pitch)),
                FixedMul(FixedMul(mag, sin(angle)), cos(pitch)),
                FixedMul(mag, -sin(pitch)), 0, 0);

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
