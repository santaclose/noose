uniform sampler2D tex;

void main()
{
	vec4 p = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = vec4(vec3(1.0 - p.rgb), p.a);
}