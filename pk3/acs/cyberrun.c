#include "zcommon.acs"

#library "cyberrun"

script 400 ENTER
{
    While (TRUE)
    {
        TakeInventory("DashCooldown",1);
        TakeInventory("JumpCooldown",1);
        TakeInventory("BoostCooldown",1);
        delay(5);
        TakeInventory("DashCooldown",1);
        TakeInventory("JumpCooldown",1);
        TakeInventory("BoostCooldown",1);
        delay(5);
        TakeInventory("DashCooldown",1);
        TakeInventory("JumpCooldown",1);
        TakeInventory("BoostCooldown",1);
        delay(5);
        TakeInventory("DashCooldown",1);
        TakeInventory("JumpCooldown",1);
        TakeInventory("BoostCooldown",1);
        GiveInventory("PlasmaGunAmmo",1);
        delay(5);
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
    if (CheckInventory("DashCooldown") == 0)
    {
        ActivatorSound("cyber/dash",255);
        ThrustThing(GetActorAngle(0)/256,42,1,0);
        ThrustThingZ(0,16,0,1);
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
