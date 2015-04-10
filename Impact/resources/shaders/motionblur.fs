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
uniform float uBlur;
uniform vec2 uV;
uniform vec2 uResolution;
uniform float uRot;

void main(void) {
  vec2 center = vec2(0.5);
  vec2 v = uV / 100.0;
  float blur = uBlur / uResolution.x;
  mat2 rot = mat2(cos(uRot), -sin(uRot), sin(uRot), cos(uRot));
  vec2 pos = (gl_TexCoord[0].xy - center) * rot + center;
  vec2 blurPos = gl_TexCoord[0].xy + v;
  vec4 sum = vec4(0.0);
  sum += texture2D(uTexture, blurPos + vec2(0.0, 0.0 * blur)) * 0.2270270270;
  sum += texture2D(uTexture, blurPos + vec2(0.0, 1.0 * blur)) * 0.1945945946;
  sum += texture2D(uTexture, blurPos + vec2(0.0, 2.0 * blur)) * 0.1216216216;
  sum += texture2D(uTexture, blurPos + vec2(0.0, 3.0 * blur)) * 0.0540540541;
  sum += texture2D(uTexture, blurPos + vec2(0.0, 4.0 * blur)) * 0.0162162162;
  gl_FragColor = texture2D(uTexture, pos) + sum * 0.6135135135;
}
