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
uniform float uBlur;
uniform vec2 uResolution;

void main(void) {
  vec2 pos = gl_TexCoord[0].xy;
  float sx = uBlur / uResolution.x;
  vec4 sum =  texture2D(uTexture, vec2(pos.x - 1.00 * sx, pos.y)) * 0.0162162162;
       sum += texture2D(uTexture, vec2(pos.x - 0.75 * sx, pos.y)) * 0.0540540541;
       sum += texture2D(uTexture, vec2(pos.x - 0.50 * sx, pos.y)) * 0.1216216216;
       sum += texture2D(uTexture, vec2(pos.x - 0.25 * sx, pos.y)) * 0.1945945946;
       sum += texture2D(uTexture, vec2(pos.x, pos.y)) * 0.227027027;
       sum += texture2D(uTexture, vec2(pos.x + 0.25 * sx, pos.y)) * 0.1945945946;
       sum += texture2D(uTexture, vec2(pos.x + 0.50 * sx, pos.y)) * 0.1216216216;
       sum += texture2D(uTexture, vec2(pos.x + 0.75 * sx, pos.y)) * 0.0540540541;
       sum += texture2D(uTexture, vec2(pos.x + 1.00 * sx, pos.y)) * 0.0162162162;
  gl_FragColor = sum;
}
