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
uniform vec2 uResolution;

uniform float uThreshold; // 0.0, 5.0, 0.41
uniform float uF; // 0.0, 4.0, 0.1


vec4 outline(vec2 uv)
{
  vec4 lines = vec4(0.30, 0.59, 0.11, 1.0) * vec4(uF, uF, uF, 1.0);
  
  float s11 = dot(texture2D(uTexture, uv + vec2(-1.0 / uResolution.x, -1.0 / uResolution.y)), lines);
  float s12 = dot(texture2D(uTexture, uv + vec2(0, -1.0 / uResolution.y)), lines);
  float s13 = dot(texture2D(uTexture, uv + vec2(1.0 / uResolution.x, -1.0 / uResolution.y)), lines);
  float s21 = dot(texture2D(uTexture, uv + vec2(-1.0 / uResolution.x, 0.0)), lines);
  float s23 = dot(texture2D(uTexture, uv + vec2(-1.0 / uResolution.x, 0.0)), lines);
  float s31 = dot(texture2D(uTexture, uv + vec2(-1.0 / uResolution.x, 1.0 / uResolution.y)), lines);
  float s32 = dot(texture2D(uTexture, uv + vec2(0, 1.0 / uResolution.y)), lines);
  float s33 = dot(texture2D(uTexture, uv + vec2(1.0 / uResolution.x, 1.0 / uResolution.y)), lines);
  float t1 = s13 + s33 + (2.0 * s23) - s11 - (2.0 * s21) - s31;
  float t2 = s31 + (2.0 * s32) + s33 - s11 - (2.0 * s12) - s13;
  vec4 col;
  if (((t1 * t1) + (t2* t2)) > uThreshold) {
    col = vec4(0.0, 0.0, 0.0, 1.0) * texture2D(uTexture, uv);
  }
  else {
    col = texture2D(uTexture, uv);
  }
  return col;
}

void main(void)
{
  gl_FragColor = outline(gl_TexCoord[0].st);
}
