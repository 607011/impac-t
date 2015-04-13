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
uniform float uRot;
uniform vec2 uV;
uniform vec2 uResolution;

varying mat2 vRot;
varying vec2 vTexCoord;

void main(void) {
  vec2 v = 0.65 * uV / uResolution.x;
  float blur = uBlur / uResolution.x;
  const float N = 5.0;
  vec4 sum = texture2D(uTexture, vTexCoord);
  sum += texture2D(uTexture, vTexCoord + v * (1.0 / N) * vRot) * 0.39894228;
  sum += texture2D(uTexture, vTexCoord + v * (2.0 / N) * vRot) * 0.35206533;
  sum += texture2D(uTexture, vTexCoord + v * (3.0 / N) * vRot) * 0.24197072;
  sum += texture2D(uTexture, vTexCoord + v * (4.0 / N) * vRot) * 0.12951760;
  sum += texture2D(uTexture, vTexCoord + v * (5.0 / N) * vRot) * 0.05399097;
  gl_FragColor = sum / 2.176486894;
}
