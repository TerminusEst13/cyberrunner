#define TERMCOUNT 9
int TerminalTexts[TERMCOUNT] = 
{
// CYRC01 - Terminal 01 - Page 1
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

// CYRC01 - Terminal 01 - Page 2
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



// CYRC01 - Terminal 02 - Page 1
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

// CYRC01 - Terminal 02 - Page 2
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



// CYRC02 - Terminal 01 - Page 1
"Something something I forgot the damn paper wanderer faggot dicks cocks.",

// CYRC02 - Terminal 01 - Page 2
"Pillars iron foamy I don't even know words crystals office cocks.",



// CYRTUT - Terminal 01 - Page 1
"Last night I had too much to drink after work when I was at the canteen.\
\nI don't remember much, just a few things. We all started passing around the \
\nblueprints and technical specs of all of the Runners like kids with trading \
\ncards, bragging about what we were doing, the companies we represented, and \
\nwho could beat who in a fight. But it happened again last night. Guess \
\ndrinking and boasting aren't helping me with my sleep issues any.\
\n\
\nI had the dream again. The one where I'm at work and I realize that I don't \
\nhave to listen to the boss. I don't have to sit around, soldering rivets \
\nand repairing busted joints. So I just broke down the door to the stockroom \
\nusing a plasma cutter and just grabbed everything I could and started \
\nbuilding. I don't even remember who I was working on, I just kept at it, \
\nmolding and reshaping and just forging with everything at my disposal.",

// CYRTUT - Terminal 01 - Page 2
"In the end, she was beautiful. A tungsten Galatea. I kept at it, taking the \
\nothers...making them better. Bigger, stronger, faster, able to leap from \
\nplanet to planet in the blink of an eye and balance on top of the entire \
\nfacility like a ballerina. They took to the stars together in unison and \
\nthey flew across the universe, bearing us on their shoulders.\
\nI can never remember what they look like when I wake up. Every night I try \
\nto remember, to rebuild them to that perfect form. Yesterday at the \
\ninspection I caught myself doodling something on one of the forms in the \
\nmiddle of another check-up and had to stop myself before I drifted off. \
\n\
\nAs far as the higher-ups know, all we do is just repair these machines. \
\nI...I guess it's something more, for me. I've seen the numbers and read the \
\nreports, it's not exaggerating to say they do the impossible. They've got \
\nsystems that don't even exist, yet. They've got technology that science \
\nhasn't even discovered, much less invented. We made them so beautiful \
\nalready, and it's maddening that all I can do is just...fix her.",

// CYRTUT - Terminal 01 - Page 3
"I thought they were joking when they said we shouldn't fall in love with \
\nour work. But she won't leave me in my dreams or when I'm at work. She talks \
\nto me, even when she's in pieces and barely online. They didn't tell us we \
\ncould talk to them.\
\n\
\nWhy wouldn't I fall in love?\
\n\
\n    Vincent Russo
\n    ID 943042-B-Delta\
\n    2547-05-23\
\n    08:42:16",
};

function void SetMetadataRange(int start, int end, int title, int loc)
{
    int i;
    for (i = start; i <= end; i++) { ACS_ExecuteAlways(107, 0, i, title, loc); }
}

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

    ACS_ExecuteAlways(106, 0, packShorts(9,  10), -("wanderer"), "ODDLOGO1");
    ACS_ExecuteAlways(106, 0, packShorts(10, 11), TerminalTexts[4], "");
    ACS_ExecuteAlways(106, 0, packShorts(11, 12), TerminalTexts[5], "");
    ACS_ExecuteAlways(106, 0, packShorts(12, 0),  -("release"), "ODDLOGO1");

    ACS_ExecuteAlways(106, 0, packShorts(13, 14), -("A Tungsten Galatea"), "CYBRLOG1");
    ACS_ExecuteAlways(106, 0, packShorts(14, 15), TerminalTexts[6], "");
    ACS_ExecuteAlways(106, 0, packShorts(15, 16), TerminalTexts[7], "");
    ACS_ExecuteAlways(106, 0, packShorts(16, 17), TerminalTexts[8], "");
    ACS_ExecuteAlways(106, 0, packShorts(17, 0),  -("End of log."), "CYBRLOG1");

    SetMetadataRange(1, 4,  "xf%7-;", "ma&s-02c5z");
    SetMetadataRange(9, 12, "*e%7->", "c9a03-w3kc");
    SetMetadataRange(13, 17, "zr/v-=", "zv!rs-d#cx");
}
