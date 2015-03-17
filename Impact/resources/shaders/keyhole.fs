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
uniform float uStretch;
uniform float uAspect;
uniform float uDarkest;
uniform vec2 uCenter;

void main(void)
{
  vec2 aspect = vec2(1.0 / uAspect, uAspect);
  vec2 coord = gl_TexCoord[0].st * aspect;
  vec2 center = vec2(uCenter.x, 1.0 - uCenter.y) * aspect;
  float dist = uStretch * distance(center, coord);
  float lightness = 1.0 - pow(dist, 1.65);
  vec3 rgb = texture2D(uTexture, gl_TexCoord[0].st).rgb;
  gl_FragColor = vec4(rgb * lightness, 1.0);
}
