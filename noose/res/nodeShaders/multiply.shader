uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
	vec4 t0 = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 t1 = texture2D(tex1, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = t0 * t1;
}