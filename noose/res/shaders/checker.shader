#define S 8. // Scale

//uniform vec2 iResolution;

void main()
{
	//vec2 uv = floor(S * gl_FragCoord.xy * vec2(iResolution.x / iResolution.y, 1.) / iResolution.xy);
	vec2 square = floor(gl_FragCoord.xy / S);// / iResolution.xx);
	gl_FragColor = vec4(vec3(mod(square.x + square.y, 2.)), 1.);
	if (gl_FragColor.r == 1.0)
		gl_FragColor.rgb = 70.0 / 255.0;
	else
		gl_FragColor.rgb = 54.0 / 255.0;
}