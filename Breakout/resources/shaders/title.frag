// Copyright (c) 2015 Oliver Lau <ola@ct.de>
// All rights reserved.

uniform sampler2D uTexture;
uniform float uT;
uniform float uMaxT;

const float tOffset = 0.5;

void main()
{
	vec2 pos = gl_TexCoord[0].xy;
	const vec2 center = vec2(0.5, 0.4);
	float v = 1 / min((uT + tOffset) / uMaxT, 1.f);
	float scale = pow(v, 5.5f);
	gl_FragColor = texture2D(uTexture, scale * (pos - center) + center);
}
