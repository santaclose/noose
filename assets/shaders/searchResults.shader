uniform float sel;
uniform float count;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    float index = floor(uv.y / (1.0 / count));
    if (index == sel)
        gl_FragColor = gl_Color.rgba + vec4(vec3(0.2), 0.0);
    else
        gl_FragColor = gl_Color;
}


/*uniform vec2 mousePos;
uniform float boxCount;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    if (mousePos.x > 1.0 || mousePos.y > 1.0 || mousePos.y < 0.0 || mousePos.x < 0.0)
        gl_FragColor = gl_Color;
    else
    {
        float boxSize = 1.0 / boxCount;
        float index = floor(mousePos.y / boxSize);
        if (uv.y > index* boxSize&& uv.y < (index + 1.0) * boxSize)
            gl_FragColor = gl_Color.rgba + vec4(vec3(0.2), 0.0);
        else
            gl_FragColor = gl_Color;
    }
}*/