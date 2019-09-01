uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float mode;

void main()
{
	float m = mod(mode, 5.0); // 0: Add, 1: Subtract, 2: Multiply, 3: Divide, 4; Layer

	vec4 t0 = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 t1 = texture2D(tex1, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));

	if (m == 0.0)
		gl_FragColor = t0 + t1;
	else if (m == 1.0)
		gl_FragColor = t0 - t1;
	else if (m == 2.0)
		gl_FragColor = t0 * t1;
	else if (m == 3.0)
		gl_FragColor = t0 / t1;
	else if (m == 4.0)
		gl_FragColor = vec4(t0.rgb * (1.0 - t1.a) + t1.rgb * t1.a, t1.a + t0.a);
}