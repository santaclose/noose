uniform sampler2D tex;

void main()
{
	vec4 t = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	float gray = dot(t.rgb, vec3(0.299, 0.587, 0.114));
	gl_FragColor = vec4(vec3(gray), t.a);
}