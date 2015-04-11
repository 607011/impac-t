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

uniform float uRot;

varying vec2 vTexCoord;
varying mat2 vRot;

void main() {
  const vec2 center = vec2(0.5);
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  vec4 texCoord = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  vRot = mat2(cos(uRot), sin(uRot), -sin(uRot), cos(uRot));
  vTexCoord = (texCoord.st - center) * vRot + center;
  gl_FrontColor = gl_Color;
}
