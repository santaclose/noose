uniform sampler2D tx;
uniform vec4 color;
uniform float tolerance;

void main()
{
	vec4 p = texture2D(tx, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));

	float r = smoothstep(1.0 - tolerance, 1.0, 1.0 - distance(p.rgb, color.rgb));
	gl_FragColor = vec4(r, r, r, p.a);
}
