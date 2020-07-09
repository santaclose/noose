uniform sampler2D tx;
uniform float side;
uniform float ratio;
uniform vec4 color;

void main()
{
	vec2 uv = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);
	vec2 targetUV;

	if (side == 0.0)
	{
		targetUV = vec2(uv.x, (1.0 + ratio) * uv.y - ratio);
		if (targetUV.y < 0.0)
			gl_FragColor = color;
		else
			gl_FragColor = texture2D(tx, targetUV);
	}
	else if (side == 1.0)
	{
		targetUV = vec2((1.0 + ratio) * uv.x, uv.y);
		if (targetUV.x > 1.0)
			gl_FragColor = color;
		else
			gl_FragColor = texture2D(tx, targetUV);
	}
	else if (side == 2.0)
	{
		targetUV = vec2(uv.x, (1.0 + ratio) * uv.y);
		if (targetUV.y > 1.0)
			gl_FragColor = color;
		else
			gl_FragColor = texture2D(tx, targetUV);
	}
	else if (side == 3.0)
	{
		targetUV = vec2((1.0 + ratio) * uv.x - ratio, uv.y);
		if (targetUV.x < 0.0)
			gl_FragColor = color;
		else
			gl_FragColor = texture2D(tx, targetUV);
	}
}