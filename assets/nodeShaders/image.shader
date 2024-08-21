uniform sampler2D tx;
uniform int flip = 0;

void main()
{
	vec4 p;
	p = float(1 - flip) * texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)) + 
		float(flip)     * texture2D(tx, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
	gl_FragColor = vec4(p);
}
