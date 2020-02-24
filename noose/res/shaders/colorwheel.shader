const float pi = 3.141592653589793238462643383279502884197169;
const float deg2rad = pi / 180.0;
const float rad2deg = 180.0 / pi;
const float circleRadius = 0.45; // in uv space

uniform int limit = 1;

void main()
{
	vec2 center = vec2(0.5, 0.5);

	//float whiteness = 1.0 - distance(fragCoord.xy, center)/radius;

	//float y = gl_FragCoord.y - center.y;
	//float x = gl_FragCoord.x - center.x;
	
	float y = gl_TexCoord[0].y - center.y;
	float x = gl_TexCoord[0].x - center.x;
	float angle = atan(y / x) * rad2deg;

	if (x < 0.0)
	{
		if (y < 0.0)
			angle -= 180.0;
		if (y > 0.0)
			angle += 180.0;
	}

	float red = 1.0 - (abs(angle) - 60.0) / 60.0;
	float green = 1.0 - (abs(angle - 120.0) - 60.0) / 60.0;

	if (angle < 0.0)
		green = (abs(angle) - 120.0) / 60.0;

	float blue = 1.0 - (abs(angle + 120.0) - 60.0) / 60.0;
	if (angle > 0.0)
		blue = (abs(angle) - 120.0) / 60.0;


	float whiteness = 1.0 - distance(center, gl_TexCoord[0].xy) / circleRadius;
	gl_FragColor = mix(clamp(vec4(red, green, blue, 1.0), 0.0, 1.0), vec4(1.0), whiteness);
	//gl_FragColor += (1.0 - whiteness) * vec4(1.0);
	//gl_FragColor = gl_FragColor * whiteness + vec4(1.0) * (1.0 - whiteness);

	//gl_FragColor = vec4(gl_TexCoord[0].x, gl_TexCoord[0].y, 0.0, 1.0); uv testing

	/*if (distance(gl_FragCoord.xy, center) > radius)
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);*/

	float l = length(vec2(x, y));
	// antialiasing
	float fadeRange = 0.007;
	if (l > circleRadius && limit == 1)
	{
		//gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		float factor = l - circleRadius;
		if (factor > fadeRange)
			factor = fadeRange;

		factor = factor / fadeRange;
		gl_FragColor *= 1.0 - factor;
	}
}