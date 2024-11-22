uniform sampler2D tx;
uniform vec2 size;

void main()
{
	vec3 a = texture2D(tx, (gl_FragCoord.xy - vec2(0, 0))/size).rgb;
	vec3 b = texture2D(tx, (gl_FragCoord.xy - vec2(1, 0))/size).rgb;
	vec3 c = texture2D(tx, (gl_FragCoord.xy - vec2(0, 1))/size).rgb;
	
	vec3 db = abs(a - b);
	vec3 dc = abs(a - c);

	gl_FragColor = vec4(max(db, dc), 1.0);
}