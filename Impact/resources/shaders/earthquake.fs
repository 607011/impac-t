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
uniform vec2 uRShift;
uniform vec2 uGShift;
uniform vec2 uBShift;
uniform float uT;
uniform float uMaxT;

float easeOutExpo(float t, float d) {
  return pow(2.0, -10.0 * t / d);
}

void main(void)
{
  vec2 pos = gl_TexCoord[0].xy;
  float intensity = easeOutExpo(uT / uMaxT, uMaxT - uT);
  vec3 rgb = vec3(
    texture2D(uTexture, uRShift * intensity + pos).r,
    texture2D(uTexture, uGShift * intensity + pos).g,
    texture2D(uTexture, uBShift * intensity + pos).b
  );
  gl_FragColor = vec4(rgb, 1.0);
}
