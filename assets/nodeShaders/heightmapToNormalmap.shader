uniform sampler2D tx;
uniform vec2 size;
uniform float maxHeight;
uniform int mode;

void main()
{
	float a = texture2D(tx, (gl_FragCoord.xy)/size).r;
	float left = texture2D(tx, (gl_FragCoord.xy - vec2(1, 0))/size).r;
	float above = texture2D(tx, (gl_FragCoord.xy - vec2(0, 1))/size).r;
	float right = texture2D(tx, (gl_FragCoord.xy + vec2(1, 0))/size).r;
	float below = texture2D(tx, (gl_FragCoord.xy + vec2(0, 1))/size).r;
	
	float dLeft = (left - a) * maxHeight;
	float dAbove = (a - above) * maxHeight;
	float dRight = (a - right) * maxHeight;
	float dBelow = (below - a) * maxHeight;

	vec3 vectorLeft = normalize(cross(vec3(1.0, 0.0, dLeft), vec3(0.0, 1.0, 0.0)));
	vec3 vectorAbove = normalize(cross(vec3(0.0, -1.0, dAbove), vec3(1.0, 0.0, 0.0)));
	vec3 vectorRight = normalize(cross(vec3(1.0, 0.0, dRight), vec3(0.0, 1.0, 0.0)));
	vec3 vectorBelow = normalize(cross(vec3(0.0, -1.0, dBelow), vec3(1.0, 0.0, 0.0)));
	vec3 normal = normalize(vectorLeft + vectorAbove + vectorRight + vectorBelow);

	vec3 outColor = (normal * 0.5) + vec3(0.5, 0.5, 0.5);

	// handle directx mode
	if (mode == 1)
		outColor.g = 1.0 - outColor.g;

	gl_FragColor = vec4(outColor, 1.0);
}