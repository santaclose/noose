#define S 5. // Scale

uniform vec2 iResolution;

void main()
{
	vec2 uv = floor(S * gl_FragCoord.xy * vec2(iResolution.x / iResolution.y, 1.) / iResolution.xy);
	gl_FragColor = vec4(vec3(mod(uv.x + uv.y, 2.)), 1.);
}