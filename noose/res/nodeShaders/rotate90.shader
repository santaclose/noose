uniform sampler2D tx;
uniform float times;

void main()
{
	float m = mod(times, 4.0);
	vec4 t0;
	if (m == 1.0)
		t0 = texture2D(tx, vec2(gl_TexCoord[0].t, gl_TexCoord[0].s));
	else if (m == 0.0)
		t0 = texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	else if (m == 2.0)
		t0 = texture2D(tx, vec2(1.0 - gl_TexCoord[0].s, gl_TexCoord[0].t));
	else
		t0 = texture2D(tx, vec2(1.0 - gl_TexCoord[0].t, 1.0 - gl_TexCoord[0].s));
	gl_FragColor = t0;
}