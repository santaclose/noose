uniform sampler2D tex;
uniform float gamma;

void main()
{
	vec4 t = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	gl_FragColor = vec4(vec3(pow(t.r, gamma), pow(t.g, gamma), pow(t.b, gamma)), t.a);
}