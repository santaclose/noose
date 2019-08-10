uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D fac;

void main()
{
	vec4 t0 = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 t1 = texture2D(tex1, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	float f = texture2D(fac, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r; // get a single channel
	gl_FragColor = f * t0 + (1.0 - f) * t1;
}