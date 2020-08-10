uniform sampler2D tx;
uniform float vertically;

void main()
{
	vec4 p;
	if (mod(vertically, 2.0) == 1.0)
		p = texture2D(tx, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
	else
		p = texture2D(tx, vec2(1.0 - gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = p;
}