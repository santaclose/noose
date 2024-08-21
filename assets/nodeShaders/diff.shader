uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
	vec4 src = texture2D(tex0, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec4 dst = texture2D(tex1, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	
	float equal = float(src.x == dst.x && src.y == dst.y && src.z == dst.z && src.w == dst.w);
	gl_FragColor = vec4(1.0 - equal, equal, 0.0, 1.0);
}