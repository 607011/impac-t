/*

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

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

*/

uniform sampler2D uTexture;
uniform float uT;
uniform float uMaxT;


float bounceEaseOut(float t, float b, float c, float d)
{
  if ((t /= d) < (1.0 / 2.75)) {
    return c*(7.5625*t*t) + b;
  }
  else if (t < (2.0 / 2.75)) {
    float postFix = t -= (1.5 / 2.75);
    return c*(7.5625*(postFix)*t + .75) + b;
  }
  else if (t < (2.5 / 2.75)) {
    float postFix = t -= (2.25 / 2.75);
    return c*(7.5625*(postFix)*t + .9375) + b;
  }
  else {
    float postFix = t -= (2.625 / 2.75);
    return c*(7.5625*(postFix)*t + .984375) + b;
  }
}

void main()
{
	vec2 pos = gl_TexCoord[0].xy;
	const vec2 center = vec2(0.5, 0.4);
  float scale = uT < uMaxT ? bounceEaseOut(uT, 0.1, 1.0, uMaxT) : 1.1;
	gl_FragColor = texture2D(uTexture, (pos - center) / scale + center);
}
