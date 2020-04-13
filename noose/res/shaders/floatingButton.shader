//uniform vec2 mousePos;
uniform float radius;

void main()
{
    float fadeRange = 0.05;
    vec2 uv = gl_TexCoord[0].xy;

    gl_FragColor.rgb = gl_Color.rgb;
    gl_FragColor.a = 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));
}