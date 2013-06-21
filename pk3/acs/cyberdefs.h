#define MODES_OPEN          511
#define MODES_ENTER         512
#define MODES_DISCONNECT    513
#define MODES_SWITCH        514

#define MODES_OPEN_CLIENT   531
#define MODES_ENTER_CLIENT  522
#define MODES_INFORM        523

#define ENDGAME_SCOREBOARD  515

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
int ReadyToExit[PLAYERMAX] = {0};
int Purged[PLAYERMAX] = {0};

#define BARFONTCOUNT 12

int BarGraphics[BARFONTCOUNT] = // These are the different graphics that make up the MPH bar.
{
    "MPHBAR1", "MPHBAR2", "MPHBAR3", "MPHBAR4", "MPHBAR5", "MPHBAR6",
    "MPHBAR7", "MPHBAR8", "MPHBAR9", "MPHBAR10", "MPHBAR11", "MPHBAR12",
};

#define RECHARGECOUNT 10

int RechargingItems[RECHARGECOUNT] = // These are the items that "recharge" over time, that are slowly removed or added to the player's inventory.
{
    "DashCooldown", "JumpCooldown", "BoostCooldown", "PlasmaGunAmmo",
    "ForceVentAmmo", "CannotIntoShotgun", "CannotIntoCarbine",
    "CannotIntoVulcan", "RedPlasmaGunAmmo", "CannotIntoSuper"
};

int RechargingTimes[RECHARGECOUNT][2] = // The max amount of an item, and whether 1 is given or -1 is taken.
{
    {5, -1}, {5, -1}, {5, -1}, {15, 1}, {210, 1}, {1, -1}, {1, -1}, {1, -1}, {15, 1}, {1, -1}
};

#define RAINBOWCOLORS 12
#define RAINBOWINTERVAL 10

int RainbowBeamActors[RAINBOWCOLORS] = // The Purge Rifle's pretty colors.
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

#define PARTICLECOUNT 12
#define PARTDIST_MIN  8.0
#define PARTDIST_MAX  32.0
 
int ParticleTypes[PARTICLECOUNT] = // The particles that are summoned from an item
{
    "CyberShotgunSparkle", "CyberCarbineSparkle", "CyberVulcanSparkle", "ForceVentSparkle",
    "TurboSparkle", "AllAmmoSparkle", "50HPSparkle", "InstagibSparkle", "25HPSparkle",
	"RedSparkle", "WhiteSparkle", "BlueSparkle",
};

#define PLACENAMECOUNT 10

int PlaceNames[PLACENAMECOUNT] = // The ranks on a scoreboard
{
    "\cfFirst",
    "\cjSecond",
    "\ceThird",
    "\cdFourth",
    "\caFifth",
    "\cnSixth",
    "\cpSeventh",
    "\ctEighth",
    "\cuNinth",
    "\csTenth",
};

// 0, 1, 2  -> x, y, z
// 3, 4     -> angle, pitch
// 5        -> last checkpoint
// 6        -> next checkpoint
int CheckpointCoords[PLAYERMAX][8];
int OldCheckpointCoords[PLAYERMAX][8];
int HasTeleported[PLAYERMAX];

#define MODECOUNT   6

#define MODE_NORMAL         1
#define MODE_COUNTDOWN      2
#define MODE_SUDDENDEATH    3
#define MODE_PURGE          4
#define MODE_SCOREBOARD     5
#define MODE_ABORT          6

int CRGameMode = MODE_NORMAL;
int CRSwitchTime = 0;
int CRSwitchTo = -1;
int CRSwitchLock;
int CRSwitched = 0;

int SuddenDeathEnd = 0;

int ClientPlace;

int CRModeNames[MODECOUNT+1] = // These are the modes activated one after each other in the finish sequence
{
    "GrossHack", // <ijon> the "GrossHack" is there so I don't have to subtract 1 every time I want to get the name of a CR mode
    "Normal", // Playing normally
    "Countdown", // Hit the finish line, time to countdown
    "Sudden Death", // Mid-Sudden Death
    "Failure Purge", // Didn't make it to the finish line, everyone gets kabloom'd
    "Scoreboard", // Display the scoreboard
    "Sudden Death Abort", // Player who hit the switch left the game
};
