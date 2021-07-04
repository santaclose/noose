uniform sampler2D tx;
uniform float radius;
uniform int samples;
uniform vec2 resolution;

#define PI 3.141592653589793238

int seed = 0;
float rand(vec2 co) {
	float val = fract(sin(dot(co + vec2(float(seed), float(seed)), vec2(12.9898, 78.233))) * 43758.5453);
	seed += 1;
	return val;
}


void main()
{
	vec2 uv = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);
	vec2 uvResRatio = 1.0 / resolution;

	vec4 averageColor = vec4(0.0);
	for (int i = 0; i < samples; i++)
	{
		float r = radius * sqrt(rand(uv));
		float theta = rand(uv) * 2.0 * PI;
		vec2 offsetVec = vec2(r * cos(theta) * uvResRatio.x, r * sin(theta) * uvResRatio.y);
		averageColor += texture2D(tx, uv + offsetVec);
	}

	averageColor /= float(samples);
	gl_FragColor = averageColor;
}