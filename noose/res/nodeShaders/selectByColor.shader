uniform sampler2D tx;
uniform vec4 color;
uniform float tolerance;

void main()
{
	vec4 p = texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));

	//float r = dot(normalize(p.rgb), normalize(color.rgb));

	float r = (1.0 - distance(p.rgb, color.rgb)) * tolerance;
	gl_FragColor = vec4(r, r, r, p.a);

	/*float r = 1.0 - length(p.rgb - color.rgb) / sqrt(3.0);
	r = smoothstep(1.0 - tolerance, 1.0, r);
	gl_FragColor = vec4(r, r, r, p.a);*/
}