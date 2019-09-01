uniform sampler2D tex;
uniform vec2 topLeft;
//uniform vec2 size;

void main()
{
	vec2 targetCoordinates = vec2(gl_TexCoord[0].s, gl_TexCoord[0].t);
	//targetCoordinates /= size;
	targetCoordinates += vec2(-topLeft.x, topLeft.y);
	targetCoordinates.t = 1.0 - targetCoordinates.t;
	gl_FragColor = texture2D(tex, targetCoordinates);
}