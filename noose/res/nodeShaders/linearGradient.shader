uniform float xResolution;

void main()
{
	gl_FragColor = vec4(vec3(gl_FragCoord.x/xResolution), 1.0);
}