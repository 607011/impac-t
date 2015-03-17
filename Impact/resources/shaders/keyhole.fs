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
uniform vec2 uCenter;
uniform float uDarkest; // 0.1
uniform float uStretch; // 2.0

void main(void)
{
    vec2 center = uCenter / uResolution;
    float dist = 1.0 - distance(center, gl_TexCoord[0].st);
    vec3 rgb = texture2D(uTexture, gl_TexCoord[0].st).rgb;
    gl_FragColor = vec4(rgb * clamp(uStretch * dist, uDarkest, 1.0), 1.0);
}
