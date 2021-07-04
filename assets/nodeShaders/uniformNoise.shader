uniform int seed;

float rand(vec2 co) {
	float val = fract(sin(dot(co + vec2(float(seed), float(seed)), vec2(12.9898, 78.233))) * 43758.5453);
	return val;
}

void main()
{
	vec2 uv = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);
	gl_FragColor.rgb = vec3(rand(uv));
	gl_FragColor.a = 1.0;
}