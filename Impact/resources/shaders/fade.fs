#version 110

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


float quadEaseInOut(float t, float d)
{
  t /= 0.5 * d;
  if (t < 1.0)
    return 0.5 * t * t;
  return -0.5 * (((t - 2.0) * (--t)) - 1.0);
}


void main()
{
  vec2 pos = gl_TexCoord[0].st;
  pos.y = 1.0 - pos.y;
  vec4 color = texture2D(uTexture, pos);
  float alpha = quadEaseInOut(uT, uMaxT);
  gl_FragColor = color * vec4(1.0, 1.0, 1.0, alpha);
}
