# Cyberrunner

## SHORT OVERVIEW
Cyberrunner is a mod for the ZDoom/Zandronum engine that essentially acts as a combination of ijon tichy's Parkmore and TerminusEst13's Superfast, taking the best of both and turning it into a full-fledged mod. In Cyberrunner, players take control of powerful and fast cyborgs/robots with large laser weapons, dodging and weaving each other's attacks, the environment, and the enemies within in order to get to the finish as quickly as possible.

## GENERAL LOOK AND FEEL
The HUD, though done in MSPaint, has a distinctly tech lean, as do all of the sounds. The aesthetic direction of Cyberrunner, unsurprisingly indicated by the name, will be towards the "Cyber". Sci-fi tech, pulse-pounding electronic music, and lots of fancy flashing lights, with plenty of emphasis in style over substance.

## FEATURE SET
In order of priority:

 1. Emphasis on the base and most primal forms of pleasure. Going fast is fun. Blowing things up is fun. Players who strive for these are rewarded, and the gameplay needs to do this.
 2. Simple to approach, difficult to master. For people who want to go do stuff right away, there's already the Cyber Dash, Cyber Lunge, and Cyber Boost. Those who want to be a true master and blaze through the air must experiment with the intricacies of Parkmore and Rocket Jumping, going from wall to wall and never touch the floor.
 3. Keep it simple, stupid. The winning formula lies primarily with Parkmore and Superfast combined. Let's not worry about extra classes, capture the flag, or etc until we get the basics hammered out first.

## TARGET AUDIENCE
???

## POSSIBLE PROBLEMS
 * Respawning. If this is to be done in the deathmatch mode, respawning puts them at a completely random place--or at the beginning. Is it possible to move a respawn point further up, upon passing a checkpoint?
 * Maps. Neither of us are especially good mappers, and this will rely very heavily on mapping--we'll need to plan out the types of obstacles, interesting things that can be done, and multiple routes for people to go on.

## MAPPER INFO

If you want to have checkpoints in your map, use script 105. It is defined like this:

  script 105 (int mode, int index, int nextToBe)

The mode argument works like this:
 - If mode is 0, teleport the player to the coordinates of their checkpoint.
 - If mode is 1, set their checkpoint. This will not happen instantly, but rather, it'll happen when they hit the ground. When they hit the ground, their position, angle, and pitch will be saved, along with the index of the next checkpoint they can trigger. If the index of the checkpoint is not higher than the index of the last checkpoint they used, and isn't the next checkpoint set, do nothing. In pseudocode:

```python
if index > lastIndex or index == nextIndex:
    lastIndex = index
    nextIndex = nextToBe
    while not onGround: delay(1)
    saveCoordinates()
```

 - Mode 2 is the same as mode 1, but silent.
 - Modes 3 and 4 are the same as 1 and 2, respectively, but will always set a checkpoint.

For most intents and purposes, you can keep nextToBe set to 0. Starting your checkpoints at index 1 is suggested, and if you have a track that loops, set the "lap line" to checkpoint 0. If you do a lapping map, you must use checkpoints, even if you don't then teleport the player to those checkpoints.
