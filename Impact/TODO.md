# TODO

## Nice to have

 - display gravity vector as a visual aid for the player
 - nicer visual effect when the racket hits a block
 - nicer visual effect for killing spree bonus
 - special blocks that ...
   - reduce illumination of the screen to a circular area around the racket/ball as if looking through a keyhole
   - blur the screen
   - darken the screen
   - lighten the screen
   - distort the screen
   - make blocks semi-transparent
   - cause a kind of asteroid shower (or other dangerous things that fall from the sky and can severely harm the racket)
   - increase inertia of racket
   - speed up/slow down ball (friction?)
   - remove gravity and friction, and set restitution of all objects to 1
 - add property to block which tells which shader to use for special effects when the block is destroyed
 - place racket on level's default position in case it was caught in between wall segments or something like that
 - integrate with Steam:
   - leaderboard for every level
   - leaderboard for campaigns
   - stats
   - achievements, e.g.:
	 - 1, 2, 4, 8, 16 ... killing sprees in one level
	 - uninterrupted play for 15, 30, 60, 90 ... minutes
	 - 1, 2, 4, 8, 16 ... hours total play time
	 - no ball lost in 1, 2, 4, 8, 16 ... levels
     - no score deduction in 1, 2, 4, 8, 16 ... levels
 - use custom cursor instead of OS's default (https://github.com/LaurentGomila/SFML/wiki/Tutorial:-Change-Cursor)
 - new playmode: best score in given time
 - new level option: constrain racket to bottom of screen as in the original Breakout game
 - new block property: racket grows or shrinks
 - new block property: ball sticks to racket and can be shot straight upwards
 - new block property: racket can shoot like a laser gun
 - two player mode as in the classic Pong game with blocks in between the players


## Maybe

 - allow resizing of screen + fullscreen mode


See [issue tracker](https://github.com/ola-ct/impac-t/issues) for bugs and further to-dos.
