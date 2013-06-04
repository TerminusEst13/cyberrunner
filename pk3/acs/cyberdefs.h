#define TID_PLAY 600

#define DASH_VEL 45

#define TIMECOUNT   3
#define TIME_START      0
#define TIME_CHECKPOINT 1
#define TIME_END        2
#define TIME_FINISH     2

#define PLACEMAX 64

int PlayerTIDs[PLAYERMAX];
int PlayerTimes[PLAYERMAX][TIMECOUNT];
int InTerminal[PLAYERMAX];
int UnfreezeDelay[PLAYERMAX];
int IsServer = 0;
int TimeDisplays[PLACEMAX][3] = {{-1, -1, -1}};  // name pointer, time, player number
int PlayerPlace[PLAYERMAX] = {-1}; 

#define BARFONTCOUNT 12

int BarGraphics[BARFONTCOUNT] =
{
    "MPHBAR1", "MPHBAR2", "MPHBAR3", "MPHBAR4", "MPHBAR5", "MPHBAR6",
    "MPHBAR7", "MPHBAR8", "MPHBAR9", "MPHBAR10", "MPHBAR11", "MPHBAR12",
};

#define RECHARGECOUNT 9

int RechargingItems[RECHARGECOUNT] = 
{
    "DashCooldown", "JumpCooldown", "BoostCooldown", "PlasmaGunAmmo",
    "ForceVentAmmo", "CannotIntoShotgun", "CannotIntoCarbine",
    "CannotIntoVulcan", "RedPlasmaGunAmmo"
};

int RechargingTimes[RECHARGECOUNT][2] = 
{
    {5, -1}, {5, -1}, {5, -1}, {15, 1}, {210, 1}, {1, -1}, {1, -1}, {1, -1}, {15, 1},
};

#define RAINBOWCOLORS 12
#define RAINBOWINTERVAL 10

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

#define PARTICLECOUNT 9
#define PARTDIST_MIN  8.0
#define PARTDIST_MAX  32.0
 
int ParticleTypes[PARTICLECOUNT] =
{
    "CyberShotgunSparkle", "CyberCarbineSparkle", "CyberVulcanSparkle", "ForceVentSparkle",
    "TurboSparkle", "AllAmmoSparkle", "50HPSparkle", "InstagibSparkle", "25HPSparkle",
};

#define PLACENAMECOUNT 10

int PlaceNames[PLACENAMECOUNT] =
{
    "\cfFirst",
    "\cjSecond",
    "\ceThird",
    "\cdFourth",
    "\caFifth",
    "\cnSixth",
    "\cpSeventh",
    "\ctEighth",
    "\ccNinth",
    "\csTenth",
};

// 0, 1, 2  -> x, y, z
// 3, 4     -> angle, pitch
// 5        -> last checkpoint
// 6        -> next checkpoint
int CheckpointCoords[PLAYERMAX][8];
int OldCheckpointCoords[PLAYERMAX][8];
int HasTeleported[PLAYERMAX];
