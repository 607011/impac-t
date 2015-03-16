# Impac't

**A Breakout/Bolo clone, augmented with a physics engine**

This game accompanies a series of articles to be published in [c't magazine](http://www.ct.de/), issue 10/15.

__The software was programmed for teaching and demonstration purposes only
and is not intended for production use. The author and Heise Medien GmbH & Co. KG
shall not be liable for any damage arising from the use of this program,
and do not accept responsibility for its completeness, correctness and fitness
for a particular purpose.__


## System requirements

 - Windows 7 or newer
 - 4 GByte RAM
 - 13 MByte free disk space
 - an OpenGL 3.x capable graphics card and driver (optimum: support for GLSL)
 - dual-core CPU @ 1,8 GHz (optimum: quad-core CPU @ 2,4 GHz)


## Solutions to common problems

### Strange graphics with Windows drivers

If graphics are missing or are looking weird, try upgrading your graphics card driver.
Always use the latest version from the graphics card manufacturer, not the one delivered
with Windows. Also check system requirements!


### Sticky graphics, low refresh rate

If the objects are not moving smoothly, your CPU is too slow (again, check system requirements!) or your
graphics card lacks performance. In the latter case, try disabling the use of shaders in the options screen.

In cases where the display is sticking when a block is going to explode,
try to reduce the number of particles per explosion.


## License

Copyright (c) 2015 [Oliver Lau](mailto:ola@ct.de), <a href="http://www.heise.de/">Heise Medien GmbH & Co. KG</a>.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see
<a href="http://www.gnu.org/licenses/">http://www.gnu.org/licenses/</a>.
