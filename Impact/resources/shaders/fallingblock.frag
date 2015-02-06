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
uniform float uAge;
uniform float uBlur;
uniform float uWeight;
uniform vec4 uColor;

void main(void) {
   vec4 sum = vec4(0.0);
   float blur = uBlur * sin(12.5f * uAge);
   vec2 pos = gl_TexCoord[0].xy;
   sum += texture2D(uTexture, vec2(pos.x - 4.0 * blur, pos.y)) * 0.05;
   sum += texture2D(uTexture, vec2(pos.x - 3.0 * blur, pos.y)) * 0.09;
   sum += texture2D(uTexture, vec2(pos.x - 2.0 * blur, pos.y)) * 0.12;
   sum += texture2D(uTexture, vec2(pos.x - 1.0 * blur, pos.y)) * 0.15;
   sum += texture2D(uTexture, vec2(pos.x + 0.0 * blur, pos.y)) * uWeight;
   sum += texture2D(uTexture, vec2(pos.x + 1.0 * blur, pos.y)) * 0.15;
   sum += texture2D(uTexture, vec2(pos.x + 2.0 * blur, pos.y)) * 0.12;
   sum += texture2D(uTexture, vec2(pos.x + 3.0 * blur, pos.y)) * 0.09;
   sum += texture2D(uTexture, vec2(pos.x + 4.0 * blur, pos.y)) * 0.05;
   gl_FragColor = sum * uColor;
}
