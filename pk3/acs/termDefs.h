#define TERMCOUNT 1
int TerminalTexts[TERMCOUNT] = 
{
"I stumble through the darkness. For some reason, I think I can see,\
\neven though I obviously can't. Whatever it is I'm feeling, I don't know \
\nwhat it is, but I think they're walls.\
\n\
\nI've been wandering for weeks. Something like that, anyway. It's hard \
\nfor me to tell time here. Where am I going? Do I still exist? There \
\nprobably isn't a way for me to get a definitive answer, although I'm \
\nstarting to believe that in a part of a universe that hasn't yet \
\nreached light. That, or I'm in the between. Whatever that is, exactly. \
\nI keep wandering.\
\n\
\nI stop. An infinitely tall pillar of indeterminate substance stands \
\nbefore me. It is bright as day, yet it emits no light upon anything \
\nelse. Trying to feel it, I walk straight through.\
\n\
\nApparently, I don't exist. Either that, or the pillar doesn't. \
\nRegardless, I can see again.\
\n\
\nI am on a rocky shore, with dust waves crashing into the rocks. The \
\npillar is gone. The sky is still as black as ever, but now a large \
\nwhite circle floats directly overhead. I guess that's the pillar now. \
\nIt dimly illuminates this new world.\
\nBesides the shore and ocean of dust, there is a red grass plain \
\n\
\nexpanding far beyond my imagination, with solid iron pillars once again \
\nextending to infinity. Those are probably what I felt. There's also a \
\nset of tracks, weaving through the pillars.\
\n\
\nIt's a path. I'll take it. I follow the tracks.",
};

script 119 OPEN clientside
{
    ACS_ExecuteAlways(106, 0, packShorts(1, 2), -("wanderer"), "ODDLOGO1");
    ACS_ExecuteAlways(106, 0, packShorts(2, 3), TerminalTexts[0]);
    ACS_ExecuteAlways(106, 0, packShorts(3, 0), -("vitalize"), "ODDLOGO1");

    ACS_ExecuteAlways(107, 0, 1, "xF%7-q", "ma&s-02c5z");
    ACS_ExecuteAlways(107, 0, 2, "xF%7-q", "ma&s-02c5z");
    ACS_ExecuteAlways(107, 0, 3, "xF%7-q", "ma&s-02c5z");
}
