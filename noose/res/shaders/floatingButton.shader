//uniform vec2 mousePos;
uniform float radius;

#define fadeRange 0.05
#define plusSignLength 0.4
#define plusSignThickness 0.07

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // circle
    gl_FragColor.rgb = gl_Color.rgb;
    gl_FragColor.a = 1.0 - smoothstep(radius - fadeRange / 2.0, radius + fadeRange / 2.0, distance(uv, vec2(0.5, 0.5)));

    // plus sign
    if (uv.x > 0.5 - plusSignThickness * 0.5 && uv.x < 0.5 + plusSignThickness * 0.5 &&
        uv.y > 0.5 - plusSignLength * 0.5 && uv.y < 0.5 + plusSignLength * 0.5 ||
        uv.x > 0.5 - plusSignLength * 0.5 && uv.x < 0.5 + plusSignLength * 0.5 &&
        uv.y > 0.5 - plusSignThickness * 0.5 && uv.y < 0.5 + plusSignThickness * 0.5)
        gl_FragColor.rgb += vec3(0.6);
}