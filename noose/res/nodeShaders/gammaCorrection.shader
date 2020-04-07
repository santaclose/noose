uniform sampler2D tex;

void main()
{
	vec4 t = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = vec4(vec3(pow(t.r, 1.0/2.2), pow(t.g, 1.0 / 2.2), pow(t.b, 1.0 / 2.2)), t.a);
}