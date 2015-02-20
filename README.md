# Impac't

A Breakout/Bolo clone, augmented with a physics engine


## TODO

 - let user select a certain level
 - amend Level::load() with the ability to load complete levels (tile map, graphics) from a zip file. The zip file must have a flat 
 - display gravity vector as a visual aid for the player
 - nicer visual effect when the racket hits a block
 - nicer visual effect for killing spree bonus
 - special blocks that ...
   - blur the screen
   - make blocks semi-transparent
   - distort the screen
   - cause a kind of asteroid shower (or other dangerous things that fall from the sky and can severely harm the racket)
   - increase inertia of racket
   - speed up/slow down ball (friction?)
   - remove gravity and friction, and set restitution of all objects to 1
 - place racket on level's default position in case it was caught in between wall segments or something like that
 - integrate with Steam:
   - leaderboard for every level
   - leaderboard for campaigns
   - stats & achievements

## License

Copyright (c) 2015 [Oliver Lau](mailto:ola@ct.de),
<a href="http://www.heise.de/">Heise Zeitschriften Verlag</a>.


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


__This software was programmed for teaching and demonstration purposes only
and is not intended for production use. The author and Heise Zeitschriften
Verlag shall not be liable for any damage arising from the use of this program,
and do not accept responsibility for its completeness, correctness and fitness
for a particular purpose.__


## Nutzungshinweise und Lizenz

Copyright (c) 2015 [Oliver Lau](mailto:ola@ct.de),
<a href="http://www.heise.de/">Heise Zeitschriften Verlag</a>.

Dieses Programm ist freie Software. Sie können es unter den Bedingungen der
<a href="http://www.gnu.org/licenses/gpl-3.0">GNU General Public License</a>,
wie von der Free Software Foundation veröffentlicht, weitergeben und/oder
modifizieren, entweder gemäß Version 3 der Lizenz oder (nach Ihrer Wahl)
jeder späteren Version.

__Diese Software wurde zu Lehr- und Demonstrationszwecken programmiert
und ist nicht für den produktiven Einsatz vorgesehen. Der Autor und der
Heise Zeitschriften Verlag haften nicht für eventuelle Schäden, die aus
der Nutzung der Software entstehen, und übernehmen keine Gewähr für ihre
Vollständigkeit, Fehlerfreiheit und Eignung für einen bestimmten Zweck.__

