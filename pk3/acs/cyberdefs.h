#define RECHARGECOUNT 8
#define TID_PLAY 600

#define TIMECOUNT   3
#define TIME_START      0
#define TIME_CHECKPOINT 1
#define TIME_END        2

int PlayerTIDs[PLAYERMAX];
int PlayerTimes[PLAYERMAX][TIMECOUNT];
int InTerminal[PLAYERMAX];
int IsServer;
