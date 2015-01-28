// Copyright (c) 2015 Oliver Lau <ola@ct.de>
// All rights reserved.

uniform sampler2D uTexture;
uniform float uAge;
uniform float uMaxAge;

void main()
{
	float v = 1.0 - (uAge / uMaxAge);
	vec2 pos = gl_TexCoord[0].xy;
	gl_FragColor = texture2D(uTexture, pos) * vec4(v, 1.0, 1.0 - v, v);
}
