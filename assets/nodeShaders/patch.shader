uniform sampler2D tx;
uniform sampler2D patchTx;
uniform vec2 topLeft;
uniform vec2 ratio;

void main()
{
	vec2 uv = vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t);

	if (uv.x > topLeft.x && uv.x < topLeft.x + ratio.x && uv.y > topLeft.y && uv.y < topLeft.y + ratio.y)
	{
		vec2 newCoords = vec2((uv.x - topLeft.x) / ratio.x, (uv.y - topLeft.y) / ratio.y);
		gl_FragColor = texture2D(patchTx, newCoords);
	}
	else
	{
		gl_FragColor = texture2D(tx, uv);
	}
}