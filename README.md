# Cyberrunner

## SHORT OVERVIEW
Cyberrunner is a mod for the ZDoom/Zandronum engine that essentially acts as a combination of ijon tichy's Parkmore (which you probably have heard of) and TerminusEst13's Superfast (which you probably haven't), taking the best of both and turning it into a full-fledged mod. In Cyberrunner, players take control of powerful and fast cyborgs/robots with large laser weapons, dodging and weaving each other's attacks, the environment, and the enemies within in order to get to the finish as quickly as possible.

## PACKAGING AND PLAYING
If you aren't a Cyberrunner developer and want to see what new unreleased shit there is, this is all you need to do:

 - Click the "ZIP" button below the Cyberrunner summary.
 - Unzip everything in the resulting ZIP file to an empty folder.
 - Navigate into the pk3/ folder.
 - Select everything (ctrl-A).
 - Put it all in a ZIP or 7-Zip (7z) file. Do *not* use RAR, since Zandronum does not support most versions of RAR.
 - Change the name of the ZIP file to "cyberrunner-git.pk3".
 - Play Cyberrunner.

If you're a developer, the process is the same, but remember to recompile ACS when you change it. ACC must be in the pk3/acs/ directory when you recompile, or else it'll blow up on the #includes (if this is wrong, tell ijon on irc.zandronum.com/#cyberrunner).

## CONTACT
Both TerminusEst13, Ijon Tichy, and various other Cyberrunner players/mappers are on #cyberrunner on irc.zandronum.com. You'll need an IRC client to get there; look at HexChat ( http://hexchat.org/ ) if you're on Windows, or XChat if you're on Linux.


## MAPPER INFO
If you want to map, there's five simple things to keep in mind.

- This is CYBERrunner, so it should be at least vaguely sci-fi in theme. Set in our solar system; outer space, the asteroid field, in a spatial anomaly, on Neptune, whatever.
- Use Doom in Hexen or UDMF format for your maps. Cyberrunner makes use of several very advanced features for mappan.
- Maps should be very wide open and spacious. This doesn't mean make a 10000x10000 field, but it should be very possible to move through at high speeds.
- We use tracker music. That means .s3m, .it, .xm, .mod, .spc, and .vgz music. This provides the wonderful synthesized instruments so reminiscent of retro sci-fi, while being VERY easy on filesize, AND gives us the wonderful ability to edit it as we please. Sorry, but we're not going to use MP3s.
- We will likely criticize it heavily. Please be able to take criticism--or failing that, please understand that we may edit your map.

If you want to have checkpoints in your map, use script 105. It is defined like this:

  script 105 (int mode, int index, int nextToBe)

The mode argument works like this:
 - Mode 0 teleports the player to the coordinates of their checkpoint.
 - Mode 1 sets their checkpoint. This will not happen instantly, but rather, it'll happen when they hit the ground. When they hit the ground, their position, angle, and pitch will be saved.
 - Mode 2 is the same as mode 1, but silent.
 - Modes 3 and 4 are the same as 1 and 2, respectively, but will always set a checkpoint.
 - Mode 5 is the finishline! It's the big finisher that activates sudden death.

The index argument is the number of the checkpoint. 105,1,1 is the first checkpoint, 105,1,2 is the second checkpoint, etc. Simple enough. Note, however, that each of the checkpoint sectors MUST have a tag of 1!

For most intents and purposes, you can keep nextToBe set to 0. Starting your checkpoints at index 1 is suggested, and if you have a track that loops, set the "lap line" to checkpoint 0. If you do a lapping map, you must use checkpoints, even if you don't then teleport the player to those checkpoints.


## JUST HOW FAST CAN THE RUNNER GO, ANYWAY?

We've calculated it to be 135494.272 mph. 0.1% away from Mach 178.

So pretty goddamn fast.
