uniform sampler2D tx;

void main()
{
	vec4 p = texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = vec4(p);
}