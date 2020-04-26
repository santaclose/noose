uniform sampler2D tx;
uniform bool xAxis;

void main()
{
	vec4 p;
	if (xAxis)
		p = texture2D(tx, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
	else
		p = texture2D(tx, vec2(1.0 - gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = p;
}