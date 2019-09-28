uniform sampler2D r;
uniform sampler2D g;
uniform sampler2D b;
uniform sampler2D a;

void main()
{
	float rr = texture2D(r, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r;
	float gg = texture2D(g, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r;
	float bb = texture2D(b, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r;
	float aa = texture2D(a, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r;
	gl_FragColor = vec4(rr, gg, bb, aa);
}