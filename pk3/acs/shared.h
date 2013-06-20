function int parkmoreOnGround(int tid)
{
    int ctid = unusedTID(13000, 18000);
    int spawned = Spawn("ParkmoreFloorChecker", GetActorX(tid), GetActorY(tid), GetActorZ(tid)-4.0, ctid);
    if (spawned) { Thing_Remove(ctid); }

    return (onGround(tid) ||
        (GetActorVelZ(tid) == 0 && !spawned));
}

function int GetParkmoreFlags(void)
{
    int ret = 0;

    ret |= !!GetCVar("cyber_cl_noledgegrab") << 0;  // bit 1

    return ret;
}
