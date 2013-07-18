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

int ChangelogCyber =
"FROM V0.1 TO V0.11:\n\
========================\n\
- Added a new Deathmatch map, CYDM03: Vertical Datapath, by Revae!\n\
- Trimmed a few unused textures.\n\
- Rephrased some obituaries due to incorrect/lame terminology.\n\
- The Shotgun's tracers now show up in non-Doom games.\n\
- Weapon balance has been completely redone using the power of MATH, rather than 'what feels right'.\n\
 ^-: Carbine now does 180 damage every 50 tics. 180/50 = 3.6, 3.6*35 = average 126 damage a second. 21 damage buff.\n\
 ^-: Shotgun does 10*4(1d3) and 15*3(1d3), 80+90 = 170, 170/40 = 4.25, 4.25*35 = average 148.75 damage a second. 8.75 damage buff.\n\
 ^-: Vulcan does two projectiles with 8 damage every 3 tics. [(2x8)/2]+(8/2) = 12 damage every 3 tics = 4, 4*35 = average 140 damage a second. 35 damage nerf.\n\
 ^-: Vulcan has had its ammo halved in LMS, with 3 tics at 500 ammo being 1500/35 = 42.85[...] seconds of firing. Its radius and height has also been reduced from 8 to 6.\n\
 ^-: Running people over has had its damage boosted from 400 to 500, to ensure sure-kills.\n\
- Running people over no longer can be reflected back and kill the Runner. That was weird.\n\
- Ammo numbers for the Shotgun, Carbine, Purge Rifle, and Hammer have been centered in the bar HUD.\n\
- The Cyberrunner tutorial, racing maps, and deathmatch maps are now accessible from the main menu.\n\
- There is now a Quake-style when-shot-connects sound indicator, toggled via cyber_cl_hitindicator.\n\
- Various tweaks across both CYRC01 and CYRC02, adjusting textures and tweaking the ACS.\n\
- The race maps now refill health and boosts upon enter.\n\
- cyber_cl_particles has been removed, in an attempt to combat the crashing that some people are having.\n\
- For vanilla jumping maps with combat, ammo drops have been added for the weapons. They replace the big ammo packs and have about a quarter of the weapon's total ammo capacity.\n\
- Nothing has changed in CYRTUT.\n\
- With Zandronum's update to 1.1, the Shotgun now has FBF_NORANDOM for normalized damage.\n\
- CYDM02 has had the side nodes removed.\n\
- CYRC02 has been rendered more open for easier navigation, with temporary additional structures for easier traversing until it can be put later in the map rotation.";