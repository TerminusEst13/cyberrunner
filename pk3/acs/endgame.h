#define ENDTIME     (35 * 15)

script ENDGAME_SCOREBOARD (void)
{
    int time;

    SetActivator(0);

    while (time < ENDTIME)
    {
        time++;
        Delay(1);
    }

    Exit_Normal(0);
}
