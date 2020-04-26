uniform sampler2D tx;
uniform int mode;

void main()
{
	vec4 p = texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	if (mode == 0) // r
		gl_FragColor = vec4(vec3(p.r), 1.0);
	else if (mode == 1) // g
		gl_FragColor = vec4(vec3(p.g), 1.0);
	else if (mode == 2) // b
		gl_FragColor = vec4(vec3(p.b), 1.0);
	else if (mode == 3) // a
		gl_FragColor = vec4(vec3(p.a), 1.0);
}