uniform sampler2D tex;

void main()
{
	vec4 t = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = vec4(vec3(t.r * .3 + t.g * .59 + t.b * .11), t.a);
}