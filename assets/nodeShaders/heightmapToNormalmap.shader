uniform sampler2D tx;
uniform vec2 size;
uniform float maxHeight;
uniform int mode;

void main()
{
	float a = texture2D(tx, (gl_FragCoord.xy - vec2(0, 0))/size).r;
	float left = texture2D(tx, (gl_FragCoord.xy - vec2(1, 0))/size).r;
	float above = texture2D(tx, (gl_FragCoord.xy - vec2(0, 1))/size).r;
	
	float dLeft = (a - left) * maxHeight;
	float dAbove = (a - above) * maxHeight;

	vec3 vectorAbove = normalize(cross(vec3(0.0, -1.0, dAbove), vec3(1.0, 0.0, 0.0)));
	vec3 vectorLeft = normalize(cross(vec3(1.0, 0.0, dLeft), vec3(0.0, 1.0, 0.0)));
	vec3 normal = normalize(vectorAbove + vectorLeft);

	vec3 outColor = (normal * 0.5) + vec3(0.5, 0.5, 0.5);

	// handle directx mode
	if (mode == 1)
		outColor.g = 1.0 - outColor.g;

	gl_FragColor = vec4(outColor, 1.0);
}