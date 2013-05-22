#define TERMCOUNT 4
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
\nelse. Trying to feel it, I walk straight through.",

"Apparently, I don't exist. Either that, or the pillar doesn't. \
\nRegardless, I can see again.\
\n\
\nI am on a rocky shore, with dust waves crashing into the rocks. The \
\npillar is gone. The sky is still as black as ever, but now a large \
\nwhite circle floats directly overhead. I guess that's the pillar now. \
\nIt dimly illuminates this new world.\
\n\
\nBesides the shore and ocean of dust, there is a red grass plain \
\nexpanding far beyond my imagination, with solid iron pillars once again \
\nextending to infinity. Those are probably what I felt. There's also a \
\nset of tracks, weaving through the pillars.\
\n\
\nIt's a path. I'll take it. I follow the tracks.",

"It's absurd how much money goes into that damned Cyberrunner thing. How \
\nmuch money do you actually need to bunch up some androids - that you \
\ndidn't even make yourself - get them to record what they see, and then\
\nsend them off to a finish line? It's just one giant waste.\
\n\
\nHow about using some of the money to fund research of new, unprecedented\
\ndiscoveries, like this anomaly? It doesn't get ratings, but potential\
\nman-made wormholes would be such a boon to mankind that the way we think\
\nof space travel would forever be redrawn.\
\n\
\nImagine sending a FTL drone out to some distant part of the galaxy -\
\nhell, even another galaxy - to have it set up a wormhole there. We'd be\
\nable to go through the wormhole, and instantly and safely be wherever\
\nthe drone landed. Retrieval would be simple. Colonization would be a\
\nbreeze. Humanity would finally leave the solar system. Teleportation\
\nwould be outed as the best space travel method out there.",

"But I guess the ratings are more important. Fucking hell, I hate the \
\npublic. I mean, I guess it would be nice to escape the solar system, but \
\nclearly watching some androids run through our scientific gold mines is\
\nmore important. Ignore the science for the flashy, fast, furious frenzy\
\nof I don't even care.\
\n\
\nMaybe if the public knew what was best for them.\
\n\
\n     Dr. Vick\
\n    2547-07-17\
\n    15:18:24",
};

script 119 OPEN clientside
{
    ACS_ExecuteAlways(106, 0, packShorts(1, 2), -("wanderer"), "ODDLOGO1");
    ACS_ExecuteAlways(106, 0, packShorts(2, 3), TerminalTexts[0]);
    ACS_ExecuteAlways(106, 0, packShorts(3, 4), TerminalTexts[1]);
    ACS_ExecuteAlways(106, 0, packShorts(4, 0), -("vitalize"), "ODDLOGO1");

    ACS_ExecuteAlways(106, 0, packShorts(5, 6), -("Grievances For Progress"), "SCILOGO1");
    ACS_ExecuteAlways(106, 0, packShorts(6, 7), TerminalTexts[2]);
    ACS_ExecuteAlways(106, 0, packShorts(7, 8), TerminalTexts[3]);
    ACS_ExecuteAlways(106, 0, packShorts(8, 0), -("End of log."), "SCILOGO1");

    ACS_ExecuteAlways(107, 0, 1, "xF%7-q", "ma&s-02c5z");
    ACS_ExecuteAlways(107, 0, 2, "xF%7-q", "ma&s-02c5z");
    ACS_ExecuteAlways(107, 0, 3, "xF%7-q", "ma&s-02c5z");
    ACS_ExecuteAlways(107, 0, 4, "xF%7-q", "ma&s-02c5z");
}
