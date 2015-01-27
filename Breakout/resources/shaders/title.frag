// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

uniform sampler2D uTexture;
uniform float uV;

void main()
{
    const float s = 115;
	const float a = 0.003;
	vec2 pos = gl_TexCoord[0].xy;
	gl_FragColor = texture2D(uTexture, pos + a * vec2(sin(s * uV), cos(s * uV)));
}
