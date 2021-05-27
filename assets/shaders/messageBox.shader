uniform vec2 margin;
uniform float separationX;
uniform float buttonHeight;

uniform vec2 mousePos;
uniform int optionCount;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    gl_FragColor = gl_Color;

    float buttonWidth = (1.0 - margin.x * 2.0 - float(optionCount - 1) * separationX) / float(optionCount);

    vec2 cursor = vec2(0.0 + margin.x, 1.0 - margin.y - buttonHeight);
    for (int i = 0; i < optionCount; i++)
    {
        float isButtonPixel = float(uv.x > cursor.x && uv.y > cursor.y && uv.x < cursor.x + buttonWidth && uv.y < cursor.y + buttonHeight);
        float mouseIsOverButton = float(mousePos.x > cursor.x && mousePos.y > cursor.y && mousePos.x < cursor.x + buttonWidth && mousePos.y < cursor.y + buttonHeight);
        gl_FragColor.rgb += vec3(isButtonPixel * 0.07);
        gl_FragColor.rgb += vec3(isButtonPixel * mouseIsOverButton * 0.07);

        cursor.x += buttonWidth + separationX;
    }
}