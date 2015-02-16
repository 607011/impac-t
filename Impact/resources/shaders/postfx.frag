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
uniform vec4 uColorMix;
uniform vec4 uColorAdd;
uniform vec4 uColorSub;

uniform vec2 uMouse;
uniform vec2 uResolution;
uniform int uIterations;
uniform float uDistort;

vec2 barrelDistortion(vec2 coord, float amt, vec2 center) {
  vec2 cc = coord - center;
  float dist = dot(cc, cc);
  return coord + cc * dist * amt;
}

float sat(float t)
{
  return clamp(t, 0.0, 1.0);
}

float linterp(float t) {
  return sat(1.0 - abs(2.0 * t - 1.0));
}

float remap(float t, float a, float b) {
  return sat((t - a) / (b - a));
}

vec3 spectrum_offset(float t) {
  vec3 ret;
  float lo = step(t, 0.5);
  float hi = 1.0 - lo;
  float w = linterp(remap(t, 1.0/6.0, 5.0/6.0));
  ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);
  return pow(ret, vec3(1.0/2.2));
}

vec4 lensDistort(vec2 uv)
{
  // vec2 center = uMouse / uResolution;
  vec2 center = vec2(0.5, 0.5);
  float reci_num_iter_f = 1.0 / float(10);
  vec3 sumcol = vec3(0.0);
  vec3 sumw = vec3(0.0);
  float t;
  vec3 w;

  // unrolled loop

  t = 0.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 1.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 2.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 3.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 4.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 5.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 6.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 7.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 8.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  t = 9.f * reci_num_iter_f;
  w = spectrum_offset(t);
  sumw += w;
  sumcol += w * texture2D(uTexture, barrelDistortion(uv, uDistort*t, center)).rgb;

  return vec4(sumcol.rgb / sumw, 1.0);
}

void main()
{
  vec2 pos = gl_TexCoord[0].xy;
  pos.y = 1.0 - pos.y;
  gl_FragColor = (lensDistort(pos) + uColorAdd - uColorSub) * uColorMix * gl_Color;
}
