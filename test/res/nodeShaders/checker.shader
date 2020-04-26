uniform float squareSize;

void main()
{
	vec2 currentBlock = floor(gl_FragCoord.xy / vec2(squareSize));
	
	if (mod(currentBlock.x + currentBlock.y, 2.0)==0.0)
		gl_FragColor = vec4(1.0);
	else
		gl_FragColor = vec4(vec3(0.0), 1.0);
}