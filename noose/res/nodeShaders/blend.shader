uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float mode;

void main()
{
	float m = mod(mode, 10.0);

	vec4 src = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 dst = texture2D(tex1, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));

	// alpha blending
	if (m == 0.0)
	{
		gl_FragColor = vec4(
			dst.rgb * (1.0 - src.a) + src.rgb * src.a,
			src.a + dst.a);
		/*gl_FragColor.a = src.a + dst.a * (1.0 - src.a);
		if (gl_FragColor.a == 0.0)
			gl_FragColor.rgb = vec3(0.0);
		else
			gl_FragColor.rgb = (src.rgb * src.a + dst.rgb * dst * (1.0 - src.a)) / gl_FragColor.a;*/
	}
	// darken
	else if (m == 1.0)
		gl_FragColor = vec4(min(src.rgb, dst.rgb), dst.a);
	// lighten
	else if (m == 7.0)
		gl_FragColor = vec4(max(src.rgb, dst.rgb), dst.a);
	// add (linear dodge)
	else if (m == 2.0)
		gl_FragColor = vec4(dst.rgb + src.rgb, dst.a);
	// multiply
	else if (m == 3.0)
		gl_FragColor = vec4(src.rgb * dst.rgb, dst.a);
	// difference
	else if (m == 4.0)
		gl_FragColor = vec4(abs(dst.rgb - src.rgb), dst.a);
	// screen
	else if (m == 8.0)
		gl_FragColor = vec4(vec3(1.0) - (vec3(1.0) - src.rgb) * (vec3(1.0) - dst.rgb), dst.a);
	// color burn
	else if (m == 5.0)
		gl_FragColor = vec4(vec3(1.0) - (1.0 - dst.rgb) / src.rgb, dst.a);
	// linear burn
	else if (m == 6.0)
		gl_FragColor = vec4(-vec3(1.0) + src.rgb + dst.rgb, dst.a);
	// color dodge
	else if (m == 9.0)
		gl_FragColor = vec4(dst.rgb / (vec3(1.0) - src.rgb), dst.a);
}