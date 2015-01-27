// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

uniform sampler2D uTexture;
uniform float uV;

void main()
{
    const float s = 115;
	const float a = 0.003;
	vec2 pos = gl_TexCoord[0].xy;
	const vec2 offset = vec2(0.5, 0.4);
	vec2 tx = pos - offset;
	gl_FragColor = texture2D(uTexture, 0.6 * (3 + cos(0.1 * uV) + sin(0.01 * uV)) * tx + offset);
}
