#include "zcommon.acs"
#include "commonFuncs.h"

script 1 (int mode, int a1, int a2)
{
    int x;
    switch (mode)
    {
      case 0:
        x = 4 * (a1 + (a2 << 8));
        x = FixedMul(x, sqrt(GetActorProperty(0, APROP_Gravity)));

        ActivatorSound("world/jumppad1", 127);
        ThrustThingZ(0, x, 0,0);
        break;
    }
}
