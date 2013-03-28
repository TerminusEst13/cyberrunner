#include "zcommon.acs"
#library "parkmore"

#include "commonFuncs.h"
#include "parkConst.h"

world int 0:MaxJumpCount;

int playerJumps[PLAYERMAX] = {0};
int hasKicked[PLAYERMAX]   = {0};
int grabbing[PLAYERMAX]    = {0};
int dontGrab[PLAYERMAX]    = {0};

int CPlayerGrounds[PLAYERMAX][2];
int PlayerGrounds[PLAYERMAX][2];
int DidSpecials[PLAYERMAX];
int playerTimers[PLAYERMAX][TIMER_COUNT];
int ClientEnterLocks[PLAYERMAX];
int IsServer;

/* Comment markers:
 *  :TURNING    - Turning scripts
 *  :MOVEMENT   - Wall-jumping, dodging scripts
 *  :DAEMONS    - The scripts that loop
 *  :USER       - The customisable crap
 *  :ASSORTED   - Where everything else goes
 */

function int parkmoreOnGround(int tid)
{
    return (onGround(tid) ||
        (GetActorVelZ(tid) == 0 && !Spawn("ParkmoreFloorChecker", GetActorX(tid), GetActorY(tid), GetActorZ(tid)-4.0)));
}


/*  :TURNING
 * Turning scripts
 */

script PARKMORE_TURN (int degrees, int factor, int direction) net clientside
{
    if (degrees < 0)
    {
        degrees *= -1;
        direction = cond(direction == CLOCKWISE, COUNTERCLOCKWISE, CLOCKWISE);
    }
    
    factor = cond(factor, factor, 4);

    int prevDegrees, addDegrees, curAngle;
    int curDegrees = 0;
    int floatDegrees = itof(degrees);
    int dirMult = cond(direction == CLOCKWISE, -1, 1);

    while (curDegrees < (floatDegrees - 0.1))
    {
        prevDegrees = curDegrees;
        addDegrees = (floatDegrees - curDegrees) / factor;
        curDegrees += addDegrees;

        SetActorAngle(0, GetActorAngle(0) + ((addDegrees * dirMult) / 360));
        Delay(1);
    }

    addDegrees = floatDegrees - curDegrees;
    SetActorAngle(0, GetActorAngle(0) + ((addDegrees * dirMult) / 360));
}

script PARKMORE_PITCH (int degrees, int factor, int direction) net clientside
{
    if (degrees < 0)
    {
        degrees *= -1;
        direction = cond(direction == CLOCKWISE, COUNTERCLOCKWISE, CLOCKWISE);
    }
    
    factor = cond(factor, factor, 4);

    int prevDegrees, addDegrees, curAngle;
    int curDegrees = 0;
    int floatDegrees = itof(degrees);
    int dirMult = cond(direction == PITCH_UP, -1, 1);

    while (curDegrees < (floatDegrees - 0.1))
    {
        prevDegrees = curDegrees;
        addDegrees = (floatDegrees - curDegrees) / factor;
        curDegrees += addDegrees;

        SetActorPitch(0, GetActorPitch(0) + ((addDegrees * dirMult) / 360));
        Delay(1);
    }

    addDegrees = floatDegrees - curDegrees;
    SetActorPitch(0, GetActorPitch(0) + ((addDegrees * dirMult) / 360));
}


script PARKMORE_SETTURN (int angle, int factor) net clientside
{
    int newAngle = mod(ftoi(GetActorAngle(0) * 360) - angle, 360);
    int direction = CLOCKWISE;

    if (newAngle > 180)
    {
        direction = COUNTERCLOCKWISE;
        newAngle = 360 - newAngle;
    }

    ACS_ExecuteAlways(PARKMORE_TURN, 0, newAngle, factor, direction);
}

script PARKMORE_SETPITCH (int pitch, int factor) net
{
    int newPitch = mod(ftoi(-GetActorPitch(0) * 360) - pitch, 360);
    int direction = PITCH_UP;

    if (newPitch > 180)
    {
        direction = PITCH_DOWN;
        newPitch = 360 - newPitch;
    }

    ACS_ExecuteAlways(PARKMORE_PITCH, 0, -newPitch, factor, direction);
}

/*  :MOVEMENT
 * Wall-jumping, dodging scripts
 */

function int hasHighJump(void)
{
    return CheckInventory("RuneHighJump") || CheckInventory("PowerHighJump")
            || CheckInventory("CyberBoostJump");
}

function int getJumpZ(void)
{
    int ret = GetActorProperty(0, APROP_JumpZ);

    ret *= (hasHighJump() + 1);
    
    return ret;
}

function void wallBounce (int type, int direction) 
{
    int forwardMult, sideMult, xyMult, zMult;
    int forward, side, up;
    int forwardx, forwardy, sidex, sidey;
    int velx, vely, velz;

    int pln = PlayerNumber();
    int angle = GetActorAngle(0);

    if (type == WB_KICKUP && hasKicked[pln]) { return; }

    switch (type)
    {
        case WB_DODGE:  xyMult =  1.0;  zMult = 0.5; break;
        case WB_KICK:   xyMult =  0.8;  zMult = 1.0; break;
        case WB_KICKUP: xyMult =  0.02; zMult = 1.3; break;
        default: return;
    }

    xyMult = FixedMul(xyMult, GetActorProperty(0, APROP_Speed));
    zMult  = FixedMul(zMult,  GetActorProperty(0, APROP_JumpZ));

    if (hasHighJump())
    {
        xyMult = FixedMul(xyMult, 1.4142);
        zMult  = FixedMul(zMult, 1.4142);
    }

    if (CheckInventory("CyberBoostSpeed")) { xyMult = FixedMul(1.4142, xyMult); }
    if (CheckInventory("CyberBoostJump"))  { zMult  = FixedMul(1.4142, zMult); }

    switch (direction)
    {
        case WD_FORWARD:  forwardMult =  1.0;   sideMult =  0.0;    break;
        case WD_FORWRITE: forwardMult =  0.707; sideMult =  0.707;  break;
        case WD_RIGHT:    forwardMult =  0.0;   sideMult =  1.0;    break;
        case WD_BACKRITE: forwardMult = -0.707; sideMult =  0.707;  break;
        case WD_BACK:     forwardMult = -1.0;   sideMult =  0.0;    break;
        case WD_BACKLEFT: forwardMult = -0.707; sideMult = -0.707;  break;
        case WD_LEFT:     forwardMult =  0.0;   sideMult = -1.0;    break;
        case WD_FORWLEFT: forwardMult =  0.707; sideMult = -0.707;  break;
        case WD_KICK:     forwardMult = -1.0;   sideMult =  0.0;    break;
        default: return;
    }
    
    forward = FixedMul(WB_XYBASE, forwardMult); 
    side    = FixedMul(WB_XYBASE, sideMult); 

    up      = FixedMul(WB_ZBASE, zMult);

    forwardx = FixedMul(cos(angle), forward);
    forwardy = FixedMul(sin(angle), forward);
    sidex = FixedMul(cos(angle-0.25), side);
    sidey = FixedMul(sin(angle-0.25), side);
    
    velx = FixedMul(forwardx + sidex, xyMult);
    vely = FixedMul(forwardy + sidey, xyMult);
    velz = up;

    switch (type)
    {
        case WB_KICK:   SetActorVelocity(0, velx, vely, velz, 0, 1); break;
        case WB_DODGE:
         if (parkmoreOnGround(0))
         {
             SetActorVelocity(0, velx + (GetActorVelX(0)/3), vely + (GetActorVelY(0)/3), velz, 0, 1); break;
         }
         else
         {
             SetActorVelocity(0, velx + (GetActorVelX(0)/2), vely + (GetActorVelY(0)/2), velz, 0, 1); break;
         }
        case WB_KICKUP: SetActorVelocity(0, velx + GetActorVelX(0), GetActorVelY(0), velz, 0, 1); break;
    }

    playerJumps[pln] = min(playerJumps[pln]+1, 1);

    if (type == WB_KICK)
    {
        switch (direction)
        {
            case WD_FORWRITE: ACS_ExecuteAlways(PARKMORE_TURN, 0, 45,  2, CLOCKWISE); break;
            case WD_RIGHT:    ACS_ExecuteAlways(PARKMORE_TURN, 0, 90,  2, CLOCKWISE); break;
            case WD_BACKRITE: ACS_ExecuteAlways(PARKMORE_TURN, 0, 135, 2, CLOCKWISE); break;
            case WD_BACK:     ACS_ExecuteAlways(PARKMORE_TURN, 0, 180, 2, CLOCKWISE); break;
            case WD_BACKLEFT: ACS_ExecuteAlways(PARKMORE_TURN, 0, 135, 2, COUNTERCLOCKWISE); break;
            case WD_LEFT:     ACS_ExecuteAlways(PARKMORE_TURN, 0, 90,  2, COUNTERCLOCKWISE); break;
            case WD_FORWLEFT: ACS_ExecuteAlways(PARKMORE_TURN, 0, 45,  2, COUNTERCLOCKWISE); break;
        }
    }

    if (type == WB_KICKUP) { hasKicked[pln] = 1; }
    else { hasKicked[pln] = 0; }

    grabbing[pln] = 0;
    dontGrab[pln] = 0;

    switch (type)
    {
        case WB_KICK: ActivatorSound("parkmore/walljump", 127); break;
        case WB_DODGE: ActivatorSound("parkmore/dodge", 127); break;
    }

    GiveInventory("KickTrail", 1);
}


script PARKMORE_WALLBOUNCE (int type, int direction, int mask)
{
    int newDir = -1;

    if (type == WB_DODGE)
    {
        int sideMove = keyDown(BT_MOVERIGHT) - keyDown(BT_MOVELEFT);
        int forwMove = keyDown(BT_FORWARD) - keyDown(BT_BACK);

        switch (direction)
        {
          case WD_FORWARD: 
            switch (sideMove)
            {
                case -1: newDir = WD_FORWLEFT; break;
                case  0: break;
                case  1: newDir = WD_FORWRITE; break;
            }
            break;
            
          case WD_BACK: 
            switch (sideMove)
            {
                case -1: newDir = WD_BACKLEFT; break;
                case  0: break;
                case  1: newDir = WD_BACKRITE; break;
            }
            break;
            
          case WD_LEFT: 
            switch (forwMove)
            {
                case -1: newDir = WD_BACKLEFT; break;
                case  0: break;
                case  1: newDir = WD_FORWLEFT; break;
            }
            break;
            
          case WD_RIGHT: 
            switch (forwMove)
            {
                case -1: newDir = WD_BACKRITE; break;
                case  0: break;
                case  1: newDir = WD_FORWRITE; break;
            }
            break;
        }

        if (newDir != -1) { direction = newDir; }
    }

    if (parkmoreOnGround(0))
    {
        if (type == WB_DODGE) { wallBounce(type, direction); }
        terminate;
    }

    if (mask == 0) { mask = 1; }

    GiveInventory(wallCheckers[direction], 1);
    Delay(1);

    if (CheckInventory("CanWallBounce") & mask)
    {
        wallBounce(type, direction);
    }

    Delay(1);
    TakeInventory("CanWallBounce", 0x7FFFFFFF);
}


script PARKMORE_LEDGEWALL (int mode)
{
    int pln = PlayerNumber();
    int curX, curY, curZ, curAngle, newZ;
    int maxLeft, maxRight;
    int highest, highestTID;
    int i, j, k, l;
    int heightTID = unusedTID(40000, 50000);

    curX = GetActorX(0);
    curY = GetActorY(0);
    curZ = GetActorZ(0);

    if (parkmoreOnGround(0) || grabbing[pln]) { terminate; }

    switch (mode)
    {
      default:
        GiveInventory("ParkmoreCheckGrab", 1);
        break;

      case LW_WALL:
        GiveInventory("OpenGrab", 1);
        GiveInventory("ParkmoreCheckWallGrab", 1);
        break;
    }

    Delay(1);

    if (grabbing[pln] || GetActorVelZ(0) >= 0 ||
        GetActorZ(0) - GetActorFloorZ(0) < 12.0)
    {
        TakeInventory("OpenGrab", 0x7FFFFFFF);
        TakeInventory("CanGrab", 0x7FFFFFFF);
        terminate;
    }

    if (CheckInventory("OpenGrab") && CheckInventory("CanGrab"))
    {
        if (mode == LW_WALL)
        {
            ACS_ExecuteAlways(PARKMORE_WALLHOLD, 0, 0,0,0);
            terminate;
        }
        
        curX -= (8 * cos(curAngle));
        curY -= (8 * sin(curAngle));
        newZ = curZ+64.0;
        curAngle = GetActorAngle(0);
        
        i = 4 * cos(curAngle);
        j = 4 * sin(curAngle);
        
        for (k = 0; k < CACOUNT; k++) { Thing_Remove(heightTID+k); }
        
        while (ThingCounts(heightTID, heightTID+CACOUNT) == 0)
        {
            newZ += 8.0;
            
            if ((newZ - 512.0) > curZ)
            {
                TakeInventory("OpenGrab", 0x7FFFFFFF);
                TakeInventory("CanGrab", 0x7FFFFFFF);
                terminate;
            }
            
            for (k = 0; k < CACOUNT; k++)
            {
                Thing_Remove(heightTID+k);
                l = k+3;
                Spawn("ParkmoreHeightFinder", curX + (i*l), curY + (j*l),
                        newZ, heightTID+k);
                PlaceOnFloor(heightTID+k);
            }
        }
        
        // we got here, so one of them spawned
        highest = GetActorZ(0);
        highestTID = 0;
        
        for (i = 0; i < CACOUNT; i++)
        {
            j = heightTID + (CACOUNT-(i+1));
            
            if (GetActorZ(j) > highest)
            {
                highest = GetActorZ(j);
                if (highestTID) { Thing_Remove(highestTID); }
                highestTID = j;
            }
            else
            {
                Thing_Remove(j);
            }
        }
        
        if (highestTID == 0)
        {
            TakeInventory("OpenGrab", 0x7FFFFFFF);
            TakeInventory("CanGrab", 0x7FFFFFFF);
            terminate;
        }
        
        heightTID = highestTID;
        ACS_ExecuteAlways(PARKMORE_LEDGEHOLD, 0, heightTID,0,0);
    }
    
    Delay(1);

    TakeInventory("OpenGrab", 0x7FFFFFFF);
    TakeInventory("CanGrab", 0x7FFFFFFF);
    terminate;
}

script PARKMORE_LEDGEHOLD (int heightTID)
{
    int pln = PlayerNumber();
    int oldSpeed, instantZ;
    int curX, curY, curZ, newZ;
    int curAngle;
    int maxLeft, maxRight;
    int ledgeMag, ledgeAngle;
    int ledgeX,  ledgeY;
    int ledgeMove;  // does two things, because of the 19 int max
    int floorHeight, floorOldHeight;
    int origDistance;
    int i;
    
    curAngle = GetActorAngle(0);
    ActivatorSound("parkmore/grabledge", 127);

    grabbing[pln] = 1;
    TakeInventory("KickTrail", 1);
    oldSpeed = GetActorProperty(0, APROP_Speed);
    SetActorProperty(0, APROP_Speed, 0.0);
    SetActorProperty(0, APROP_Gravity, 0);
    SetActorVelocity(0, 24*cos(curAngle),24*sin(curAngle),9.0, 0, 1);

    newZ = GetActorZ(heightTID) - 36.0;

    Delay(1);
    SetActorVelocity(0, 0,0,0, 0, 1);

    curX = GetActorX(0);
    curY = GetActorY(0);
    curZ = GetActorZ(0);

    floorHeight = GetActorFloorZ(heightTID);

    ledgeX       = GetActorX(heightTID) - curX;
    ledgeY       = GetActorY(heightTID) - curY;
    origDistance = magnitudeTwo(ftoi(ledgeX), ftoi(ledgeY));

    i =  0;
    i =  (keyDown(BT_FORWARD) << 0);
    i |= (keyDown(BT_BACK)    << 1);
    i |= (keyDown(BT_JUMP)    << 2);

    while (1)
    {
        floorOldHeight = floorHeight;
        floorHeight = GetActorFloorZ(heightTID);

        if (!PlayerInGame(pln) || isDead(0)) { break; }
        if (GetActorZ(0) - GetActorFloorZ(0) <= 4.0) { break; }
        if (abs(floorOldHeight - floorHeight) > 16.0)
        {
            SetActorVelocity(0, 0,0,0, 0,0);
            break;
        }

        newZ = GetActorZ(heightTID) - 44.0;
        PlaceOnFloor(heightTID);

        if (curZ != newZ)
        {
            if ((abs(curZ - newZ) < 8.0) || instantZ) { curZ = newZ; }
            else { curZ += (8.0 * sign(newZ - curZ)); }
        }
        else { instantZ = 1; }

        curX = GetActorX(0);
        curY = GetActorY(0);

        ledgeX     = GetActorX(heightTID) - curX;
        ledgeY     = GetActorY(heightTID) - curY;
        ledgeMag   = magnitudeTwo(ftoi(ledgeX), ftoi(ledgeY));
        ledgeAngle = VectorAngle(ledgeX, ledgeY);

        maxLeft  = ledgeAngle - 0.25;
        maxRight = ledgeAngle + 0.25;

        curAngle = GetActorAngle(0);
        ledgeMove = 0;
        
        if (maxLeft < 0)
        {
            if (curAngle > (0.5 + maxLeft)) { curAngle -= 1.0; }
        }
        else if (maxRight > 1.0)
        {
            if (curAngle < (maxRight - 0.5)) { curAngle += 1.0; }
        }
        
        if (maxLeft > curAngle)  { ledgeMove = 1; }
        if (maxRight < curAngle) { ledgeMove = 1; }

        SetActorPosition(0, curX, curY, curZ, 0);

        if (curZ - GetActorZ(0) > 16.0)
        {
            Spawn("FingerCrunch", GetActorX(0), GetActorY(0), GetActorZ(0)+24.0);
            break;
        }

        ledgeX /= ledgeMag;
        ledgeY /= ledgeMag;

        SetActorVelocity(0, 24*ledgeX, 24*ledgeY,0, 0, 0);

        if (keyPressed(BT_BACK) && !ledgeMove)
        {
            break;
        }
        if (keyPressed(BT_BACK) && ledgeMove)
        {
            for (i = 0; i < 2; i++)
            {
                curZ += 12.0;
                SetActorPosition(0, curX, curY, curZ, 0);
                Delay(1);
            }
            SetActorVelocity(0, -2*cos(curAngle),-2*sin(curAngle),9.0, 0, 1);
            break;
        }
        if ((keyPressed(BT_FORWARD) && !ledgeMove) || (i & 1))  // i &= keyDown(BT_FORWARD);
        {                                                       // right before while (1)
            for (i = 0; i < 2; i++)
            {
                curZ += 12.0;
                SetActorPosition(0, curX, curY, curZ, 0);
                Delay(1);
            }
            SetActorVelocity(0, 2*cos(curAngle),2*sin(curAngle),9.0, 0, 1);
            break;
        }
        if (keyPressed(BT_FORWARD) && ledgeMove)
        {
            break;
        }
        
        i = 0;

        if (!ledgeMove)  // if not facing the other way
        {
            ledgeMove = keyDown(BT_MOVELEFT) - keyDown(BT_MOVERIGHT); 
            ledgeMove *= 4;

            SetActorVelocity(heightTID, ledgeMove*cos(0.3+curAngle),ledgeMove*sin(0.3+curAngle),0, 0, 0);
            SetActorVelocity(0, (4*ledgeMove)*cos(0.25+curAngle),(4*ledgeMove)*sin(0.25+curAngle),0, 1, 1);

            if (ledgeMove)
            {
                ledgeX     = curX - GetActorX(heightTID);
                ledgeY     = curY - GetActorY(heightTID);
                ledgeAngle = VectorAngle(ledgeX, ledgeY);

                ledgeX -= cos(ledgeAngle)*origDistance;
                ledgeY -= sin(ledgeAngle)*origDistance;

                SetActorVelocity(heightTID, ledgeX,ledgeY,0, 1, 0);
            }
        }

        Delay(1);
    }

    SetActorProperty(0, APROP_Gravity, 1.0);
    SetActorProperty(0, APROP_Speed, oldSpeed);
    Thing_Remove(heightTID);
    grabbing[pln] = 0;
    dontGrab[pln] = 1;
}

script PARKMORE_WALLHOLD (void)
{
    int pln = PlayerNumber();
    int oldSpeed;
    int curX, curY, curZ;
    int origAngle, curAngle;
    int maxLeft, maxRight;
    int facingWall;
    int wallHit = 1, oldWallHit = 1;
    int i;
    
    curAngle = GetActorAngle(0);
    origAngle = curAngle;

    grabbing[pln] = 1;
    TakeInventory("KickTrail", 1);
    oldSpeed = GetActorProperty(0, APROP_Speed);

    SetActorProperty(0, APROP_Speed, 0.0);
    SetActorProperty(0, APROP_Gravity, 0);
    SetActorVelocity(0, 48*cos(curAngle),48*sin(curAngle),9.0, 0, 0);

    Delay(1);

    curAngle = GetActorAngle(0);
    curX = GetActorX(0);
    curY = GetActorY(0);
    curZ = GetActorZ(0);

    while (1)
    {
        if (!PlayerInGame(pln) || isDead(0)) { break; }
        if (GetActorZ(0) - GetActorFloorZ(0) <= 4.0) { break; }

        curX = GetActorX(0);
        curY = GetActorY(0);
        curZ = GetActorZ(0);
        curAngle = GetActorAngle(0);

        maxLeft  = origAngle - 0.25;
        maxRight = origAngle + 0.25;

        curAngle = GetActorAngle(0);
        facingWall = 1;

        oldWallHit  = wallHit;
        wallHit     = CheckInventory("CanGrab");
        TakeInventory("CanGrab", 0x7FFFFFFF);
        TakeInventory("ParkmoreAngleIndicator", 0x7FFFFFFF);
        GiveInventory("ParkmoreAngleIndicator", ftoi(origAngle * 360));
        GiveInventory("ParkmoreCheckWallGrab2", 1);

        if (!(wallHit || oldWallHit)) { break; }
         
        if (maxLeft < 0)
        {
            if (curAngle > (0.5 + maxLeft)) { curAngle -= 1.0; }
        }
        else if (maxRight > 1.0)
        {
            if (curAngle < (maxRight - 0.5)) { curAngle += 1.0; }
        }
        
        if (maxLeft > curAngle)  { facingWall = 0; }
        if (maxRight < curAngle) { facingWall = 0; }

        SetActorPosition(0, curX, curY, curZ, 0);

        if (facingWall) { SetActorVelocity(0, 0,0,-0.4, 0, 0); }
        else            { SetActorVelocity(0, 0,0,-1.6, 0, 0); }

        if ((keyPressed(BT_BACK)    && !facingWall) ||
            (keyPressed(BT_FORWARD) && facingWall))
        {
            break;
        }

        if ((keyPressed(BT_FORWARD) && !facingWall) || keyPressed(BT_JUMP))
        {
            ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_KICK, WD_FORWARD, 0);
            addTimer(pln, TIMER_BOUNCED, 2);
            break;
        }

        if (keyPressed(BT_BACK) && facingWall)
        {
            ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_KICK, WD_BACK, 0);
            addTimer(pln, TIMER_BOUNCED, 2);
            break;
        }

        Delay(1);
    }

    SetActorProperty(0, APROP_Gravity, 1.0);
    SetActorProperty(0, APROP_Speed, oldSpeed);
    grabbing[pln] = 0;
    dontGrab[pln] = 1;
}


/*
function void MultiJump(int countJump, int force)
{
    int pln = PlayerNumber();
    int jumpHeight = getJumpZ();

    int forward, side, up;
    int forwardx, forwardy, sidex, sidey;
    int velx, vely, velz;
    int angle = GetActorAngle(0);

    if ((force != 1) && (playerJumps[pln] + countJump > MaxJumpCount)) { return; }
    if (playerJumps[pln] == 0) { return; }
    
    forward = keyDown(BT_FORWARD) - keyDown(BT_BACK);
    forward *= cond(keyDown(BT_FORWARD), JUMP_FORWARD, JUMP_BACK);

    side    = keyDown(BT_MOVERIGHT) - keyDown(BT_MOVELEFT);
    side    *= JUMP_SIDE;

    forwardx = FixedMul(cos(angle), forward);
    forwardy = FixedMul(sin(angle), forward);

    sidex = FixedMul(cos(angle-0.25), side);
    sidey = FixedMul(sin(angle-0.25), side);
    
    velx = forwardx + sidex;
    vely = forwardy + sidey;

    playerJumps[pln] += countJump; 

    if (velx || vely)
    {
        SetActorVelocity(0, velx, vely, jumpHeight, 0, 1);
    }
    else
    {
        SetActorVelocity(0, GetActorVelX(0), GetActorVelY(0), FixedMul(jumpHeight, 1.2), 0, 1);
    }
}
*/

function void MultiJump(int countJump, int force)
{
    int pln = PlayerNumber();
    int jumpHeight = getJumpZ();

    if ((force != 1) && (MaxJumpCount >= 0) && (playerJumps[pln] + countJump > MaxJumpCount)) { return; }
    if (playerJumps[pln] == 0) { return; }

    playerJumps[pln] += countJump; 

    SetActorVelocity(0, GetActorVelX(0), GetActorVelY(0), jumpHeight, 0, 1);
}

function void Lunge(int force)
{
    int pln = PlayerNumber();
    int jumpHeight = FixedMul(getJumpZ(), LUNGE_ZMULT);
    int velx, vely, velz;
    int angle = GetActorAngle(0);

    if ((force != 1) && (playerJumps[pln] != 0)) { return; }

    playerJumps[pln] += 1; 

    velX = FixedMul(cos(angle), LUNGE_FORWARD);
    velY = FixedMul(sin(angle), LUNGE_FORWARD);
    velZ = jumpHeight;

    SetActorVelocity(0, velX, velY, velZ, 0, 1);
}

function void HighJump(int force)
{
    int pln = PlayerNumber();
    int jumpHeight = FixedMul(getJumpZ(), HIJUMP_ZMULT);
    int velx, vely, velz;
    int angle = GetActorAngle(0);

    if ((force != 1) && (playerJumps[pln] > 0)) { return; }

    playerJumps[pln] = max(playerJumps[pln]+1, 1);; 

    velX = FixedMul(cos(angle), -HIJUMP_BACK);
    velY = FixedMul(sin(angle), -HIJUMP_BACK);
    velZ = jumpHeight;

    SetActorVelocity(0, velX, velY, velZ, 0, 1);
    ACS_ExecuteAlways(PARKMORE_TURN, 0, 180, 6, random(0,1));
}

/*  :DAEMONS
 * The scripts that loop
 */

script PARKMORE_OPEN open
{
    if (GetCVar("parkmore_jumpcount") == 0)
    {
        ConsoleCommand("set parkmore_jumpcount 2");
        ConsoleCommand("archivecvar parkmore_jumpcount");
    }

    IsServer = 1;

    int cjumps, oldcjumps;
    
    while (1)
    {
        oldcjumps = cjumps;
        cjumps = GetCVar("parkmore_jumpcount");

        if (cjumps != oldcjumps) { MaxJumpCount = cjumps; }

        if (!GetCvar("compat_clientssendfullbuttoninfo")) { ConsoleCommand("set compat_clientssendfullbuttoninfo 1"); }
        Delay(1);
    }
}

function int getTimer(int pln, int which)
{
    return playerTimers[pln][which];
}

function void addTimer(int pln, int which, int add)
{
    if (add) { playerTimers[pln][which] = add; }
}

function void addCTimers(int pln)
{
    int i = max(0, defaultCVar("parkmore_dodgewindow",  8));
    int j = max(0, defaultCVar("parkmore_hijumpwindow", 4));

    addTimer(pln, TIMER_CFORWARD,  keyPressed(BT_FORWARD)   * i);
    addTimer(pln, TIMER_CRIGHT,    keyPressed(BT_MOVERIGHT) * i);
    addTimer(pln, TIMER_CBACK,     keyPressed(BT_BACK)      * i);
    addTimer(pln, TIMER_CLEFT,     keyPressed(BT_MOVELEFT)  * i);
    addTimer(pln, TIMER_HBACK,     keyPressed(BT_BACK)      * j);
}

function int tickTimer(int pln, int timerNum)
{
    int i = max(playerTimers[pln][timerNum]-1, 0);
    playerTimers[pln][timerNum] = i;
    return i;
}

function void tickTimers(int pln)
{
    int i;
    for (i = 0; i < TIMER_COUNT; i++)
    {
        tickTimer(pln, i);
    }
}

function void printTimers(int pln)
{
    int i, j, printstr;
    for (i = 0; i < TIMER_COUNT; i++)
    {
        j = playerTimers[pln][i];
        printStr = StrParam(s:printStr, d:i, s:":", d:!!j, s:"  ");
    }

    Print(s:printStr);
}


script PARKMORE_ENTER enter
{
    int pln = PlayerNumber();
    int ground, wasGround, didSpecial;
    int inWater, wasInWater;
    int i;
    int direction, dDirection;

    while (PlayerInGame(pln))
    {
        if (isDead(0))
        {
            playerJumps[pln] = 0;
            hasKicked[pln] = 0;
            grabbing[pln] = 0;
            dontGrab[pln] = 0;
            TakeInventory("KickTrail", 1);
            Delay(1);
            continue;
        }

        wasGround = ground;
        ground = parkmoreOnGround(0);

        PlayerGrounds[pln][0] = ground;
        PlayerGrounds[pln][1] = wasGround;

        wasInWater = inWater;
        inWater = CheckInventory("WaterIndicator");

        direction = getDirection();

        if (CheckInventory("NoParkour"))
        {
            TakeInventory("KickTrail", 1);
            if (ground)
            {
                playerJumps[pln] = 0;
                hasKicked[pln] = 0;
                grabbing[pln] = 0;
                dontGrab[pln] = 0;
            }
            else
            {
                playerJumps[pln] = max(1, playerJumps[pln]);
            }
            Delay(1);
            continue;
        }

        if (!(getTimer(pln, TIMER_BOUNCED) || wasGround) && keyPressed(BT_JUMP))
        {
            dDirection = -1;

            switch (direction)
            {
              case DIR_NW: dDirection = WD_FORWLEFT;    break;
              case DIR_N:  dDirection = WD_FORWARD;     break;
              case DIR_NE: dDirection = WD_FORWRITE;    break;
              case DIR_SW: dDirection = WD_BACKLEFT;    break;
              case DIR_S:  dDirection = WD_BACK;        break;
              case DIR_SE: dDirection = WD_BACKRITE;    break;
              case DIR_W:  dDirection = WD_LEFT;        break;
              case DIR_E:  dDirection = WD_RIGHT;       break;
            }

            if (dDirection != -1)
            {
                ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_KICK, dDirection, 0);
                if (dDirection == WD_FORWARD)
                {
                    ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_KICKUP, WD_KICK, 2);
                }
                addTimer(pln, TIMER_BOUNCED, 2);
            }
        }

        didSpecial = 0;

        if (ground || inWater)
        {
            playerJumps[pln] = 0;
            hasKicked[pln] = 0;
            grabbing[pln] = 0;
            dontGrab[pln] = 0;
        }
        else
        {
            if (keyPressed(BT_JUMP) && !DidSpecials[pln] && !grabbing[pln])
            {
                MultiJump(1, 0);
            }

            playerJumps[pln] = max(1, playerJumps[pln]);

            if (!(ground || wasGround))
            {
                if (keyDown(BT_CROUCH) && !grabbing[pln] && !dontGrab[pln])
                {
                    ACS_ExecuteAlways(PARKMORE_LEDGEWALL, 0, LW_WALL,0,0);
                }
                else if ((GetActorVelZ(0) < 0) && !grabbing[pln] && !dontGrab[pln])
                {
                    ACS_ExecuteAlways(PARKMORE_LEDGEWALL, 0, LW_LEDGE,0,0);
                }
            }
        }

        tickTimer(pln, TIMER_BOUNCED);
        DidSpecials[pln] = max(0, DidSpecials[pln]-1);

        Delay(1);
    }
}

script PARKMORE_ENTER2 enter clientside
{
    int pln = PlayerNumber();
    int dodgeDir, pukeStr;
    int ground, wasGround;
    int myLock = ClientEnterLocks[pln] + 1;

    ClientEnterLocks[pln] = myLock;

    SetHudSize(480, 360, 1);
    HudMessage(s:"Type 'phelp' for Parkmore help.";
            HUDMSG_FADEOUT, 1337, CR_WHITE, 240.4, 70.0, 3.0, 2.0);

    while (ClientEnterLocks[pln] == myLock)
    {
        dodgeDir = -1;

        wasGround = ground;
        ground = parkmoreOnGround(0);

        CPlayerGrounds[pln][0] = ground;
        CPlayerGrounds[pln][1] = wasGround;

        if (!getTimer(pln, TIMER_DIDDODGE))
        {
            if (keyPressed(BT_MOVELEFT) && getTimer(pln, TIMER_CLEFT))
            {
                dodgeDir = WD_LEFT;
            }
            else if (keyPressed(BT_FORWARD) && getTimer(pln, TIMER_CFORWARD))
            {
                dodgeDir = WD_FORWARD;
            }
            else if (keyPressed(BT_MOVERIGHT) && getTimer(pln, TIMER_CRIGHT))
            {
                dodgeDir = WD_RIGHT;
            }
            else if (keyPressed(BT_BACK) && getTimer(pln, TIMER_CBACK))
            {
                dodgeDir = WD_BACK;
            }

            if (dodgeDir != -1)
            {
                addTimer(pln, TIMER_DIDDODGE, 2);

                /*if (IsServer)  // is serverside anyway, let's not go through hoops
                {
                    ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_DODGE, dodgeDir, 0);
                }*/
                ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_DODGE, dodgeDir, 0);
                //else
                
                if (!IsServer)
                {
                    pukeStr = StrParam(s:"puke -", d:PARKMORE_REQUESTDODGE, s:" ", d:dodgeDir);
                    ConsoleCommand(pukeStr);
                }
            }
        }

        if (keyPressed(BT_JUMP))
        {
            if (getTimer(pln, TIMER_HBACK) > 0) 
            {
                if (CPlayerGrounds[pln][1])
                {
                    playerJumps[pln]--;
                    DidSpecials[pln] = 2;
                }
                
                if (!IsServer)
                {
                    pukeStr = StrParam(s:"puke -", d:PARKMORE_REQUESTDODGE, s:" 0 1");
                    ConsoleCommand(pukeStr);
                }
                else
                {
                    HighJump(0);
                }
            }
        }

        tickTimer(pln, TIMER_CFORWARD);
        tickTimer(pln, TIMER_CRIGHT);
        tickTimer(pln, TIMER_CBACK);
        tickTimer(pln, TIMER_CLEFT);
        tickTimer(pln, TIMER_HBACK);
        addCTimers(pln);

        tickTimer(pln, TIMER_DIDDODGE);

        Delay(1);
    }

}

script PARKMORE_REQUESTDODGE (int direction, int hijump) net
{
    int pln;

    if (hijump)
    {
        if (PlayerGrounds[pln][1])
        {
            playerJumps[pln]--;
            HighJump(0);
            DidSpecials[pln] = 2;
        }
    }
    else
    {
        ACS_ExecuteAlways(PARKMORE_WALLBOUNCE, 0, WB_DODGE, direction, 0);
    }
}



/*  :USER
 * The customisable crap
 */

script PARKMORE_TOGGLE (int which) net
{
    switch (which)
    {
        case 0:
            if (CheckInventory("NoParkour")) { TakeInventory("NoParkour", 0x7FFFFFFF); }
            else { GiveInventory("NoParkour", 1); }
            break;
        
        case 1:
            GiveInventory("NoParkour", 1);
            break;

        case -1:
            TakeInventory("NoParkour", 0x7FFFFFFF);
            break;
    }

    if (CheckInventory("NoParkour")) { Print(s:"Parkmore is \cgDISABLED\c-."); }
    else { Print(s:"Parkmore is \cdENABLED\c-."); }
}

script PARKMORE_HELP (void) net clientside
{
    int i, j, k, l, signal;

    SetHudSize(480, 360, 1);

    SetFont("BIGFONT");
    HudMessage(s:"PARKMORE";
            HUDMSG_FADEINOUT|HUDMSG_LOG, HMSG_BASE, CR_YELLOW, 240.4, 20.1, 1.0, 1.0, 2.0);

    SetFont("SMALLFONT");
    HudMessage(s:"Yep, another Parkour mod"; 
            HUDMSG_FADEINOUT, HMSG_BASE+1, CR_BRICK, 240.4, 48.1, 1.0, 1.0, 2.0);

    Delay(35);

    SetFont("BIGFONT");
    HudMessage(s:"PARKMORE";
            HUDMSG_PLAIN, HMSG_BASE, CR_YELLOW, 240.4, 20.1, 0.0);

    SetFont("SMALLFONT");
    HudMessage(s:"Yep, another Parkour mod"; 
            HUDMSG_PLAIN, HMSG_BASE+1, CR_BRICK, 240.4, 48.1, 0.0);

    HudMessage(s:HELPMSGS[0]; 
            HUDMSG_FADEINOUT, HMSG_BASE+2, CR_WHITE, 240.4, 120.1, 1.0, 1.0, 0.0);

    Delay(35);

    for (i = 0; i < HELPMSGCOUNT; i++)
    {
        j = HELPTIMES[i];
        HudMessage(s:HELPMSGS[i]; 
                HUDMSG_FADEOUT|HUDMSG_LOG, HMSG_BASE+2, CR_WHITE, 240.4, 120.1, 2.0, 2.0);

        for (k = 0; k < j; k++)
        {
            HudMessage(s:HELPMSGS[i]; 
                    HUDMSG_FADEOUT, HMSG_BASE+2, CR_WHITE, 240.4, 120.1, 2.0, 2.0);

            HudMessage(s:"\cf", d:j-k, s:"\c- second", s:cond(j-k != 1, "s", ""),
                       s:" left\n\ck", d:HELPMSGCOUNT-(i+1), s:"\c- screen",
                       s:cond(HELPMSGCOUNT-(i+1) != 1, "s", ""), s:" left\nHit \ca",
                       k:"+crouch", s:"\c- to end this\nHit \ca", k:"+moveleft",
                       s:"\c- and \ca", k:"+moveright", s:"\c- at the same time to",
                       s:" skip to the next screen";
                    HUDMSG_FADEOUT, HMSG_BASE+3, CR_GREEN, 240.4, 320.2, 1.0, 0.5);

            for (l = 0; l < 35; l++)
            {
                if (keyPressed(BT_CROUCH)) { signal = 2; break; }
                if (keyDown(BT_MOVELEFT | BT_MOVERIGHT))
                {
                    if (signal != -1) { signal = 1; break; }
                }
                else
                {
                    signal = 0;
                }
                Delay(1);
            }

            if (signal == 1) { signal = -1; break; }
            if (signal == 2) { break; }
        }
        if (signal == 2) { break; }
    }

    SetFont("BIGFONT");
    HudMessage(s:"PARKMORE";
            HUDMSG_FADEOUT, HMSG_BASE, CR_YELLOW, 240.4, 20.1, 0.0, 2.0);

    SetFont("SMALLFONT");
    HudMessage(s:"Yep, another Parkour mod"; 
            HUDMSG_FADEOUT, HMSG_BASE+1, CR_BRICK, 240.4, 48.1, 0.0, 2.0);
}



/*  :ASSORTED
 * Where everything else goes
 */

script PARKMORE_ASSORTED (int type, int a1, int a2)
{
    int pln = PlayerNumber();
    int x,y,z, ang, offset, tid;
    int i, j;

    switch (type)
    {
      case 0:
        x = GetActorX(0);
        y = GetActorY(0);
        z = GetActorZ(0);

        for (i = 0; i < a2; i++)
        {
            ang = random(0.0, 1.0);
            
            j = sqrt(itof(a1)/8);
            offset = random(0.0, j);
            offset = quad(1.0, 0.0, 0.0, offset);
            offset *= randSign();
            offset += itof(a1);
         
            Spawn("ParkmoreHeightTrail", x+FixedMul(offset,cos(ang)), y+FixedMul(offset,sin(ang)), z, tid);
        }
        break;

      case 1:
        SetResultValue(CheckInventory("ParkmoreAngleIndicator"));
        break;
    }
}
