uniform sampler2D tex;
uniform vec2 frame;
uniform vec4 color;

void main()
{
	vec2 uv = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);

	vec2 targetUV = vec2((1.0 + 2.0*frame.x)*uv.x - frame.x, (1.0 + 2.0*frame.y)*uv.y - frame.y);

	if (targetUV.x < 0.0 || targetUV.y < 0.0 || targetUV.y > 1.0 || targetUV.x > 1.0)
		gl_FragColor = color;
	else
		gl_FragColor = texture2D(tex, targetUV);
}