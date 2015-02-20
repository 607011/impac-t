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
uniform float uT;
uniform float uMaxT;
uniform float uMinScale;
uniform float uMaxScale;

void main()
{
  const vec2 center = vec2(0.5, 0.245);
  vec4 total = vec4(0.0);
  vec4 grabPixel;
  float scale = clamp(uMinScale + uT / uMaxT * (uMaxScale - uMinScale), uMinScale, uMaxScale);
  vec2 pos = (gl_TexCoord[0].st - center) / scale + center;
  pos.y = 1.0 - pos.y;
  total += texture2D(uTexture, pos + vec2(-1.0, -1.0) / uResolution);
  total += texture2D(uTexture, pos + vec2(1.0, -1.0) / uResolution);
  total += texture2D(uTexture, pos + vec2(1.0, 1.0) / uResolution);
  total += texture2D(uTexture, pos + vec2(-1.0, 1.0) / uResolution);
  grabPixel = texture2D(uTexture, pos + vec2(0.0, -1.0) / uResolution);
  total += grabPixel * 2.0;
  grabPixel = texture2D(uTexture, pos + vec2(0.0, 1.0) / uResolution);
  total += grabPixel * 2.0;
  grabPixel = texture2D(uTexture, pos + vec2(-1.0, 0.0) / uResolution);
  total += grabPixel * 2.0;
  grabPixel = texture2D(uTexture, pos + vec2(1.0, 0.0) / uResolution);
  total += grabPixel * 2.0;
  grabPixel = texture2D(uTexture, pos);
  total += grabPixel * 4.0;
  total *= 1.0 / 16.0;
  if (total.a < 0.5)
    total *= vec4(0.0, 0.0, 0.0, 1.0 / 0.5);
  gl_FragColor = total * vec4(1.0, 1.0, 1.0, 0.5);
}
