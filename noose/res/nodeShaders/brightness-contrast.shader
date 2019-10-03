uniform float b;
uniform float c;
uniform sampler2D tex;

void main()
{
	vec4 color = texture2D(tex, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	vec3 colorContrasted = (color)* c;
	vec3 bright = colorContrasted + vec3(b, b, b);
	gl_FragColor.rgb = bright;
	gl_FragColor.a = color.a;
}