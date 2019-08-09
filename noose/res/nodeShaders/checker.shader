uniform vec4 colorA;
uniform vec4 colorB;
uniform float squareSize;

void main()
{
	vec2 currentBlock = floor(gl_FragCoord / vec2(squareSize));
	
	if (mod(currentBlock.x + currentBlock.y, 2.0)==0.0)
		gl_FragColor = colorA;
	else
		gl_FragColor = colorB;
}