uniform sampler2D tex0;
uniform sampler2D fac;

void main()
{
	vec4 color = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	float maskValue = texture2D(fac, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t)).r; // get a single channel

	gl_FragColor.a = min(color.a, maskValue);
	gl_FragColor.rgb = color.rgb;
}