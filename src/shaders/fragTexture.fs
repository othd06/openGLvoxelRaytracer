#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenTex;

void main()
{
    vec2 uv = UV*0.5+vec2(0.5, 0.5);
    //FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    //FragColor = vec4(UV, 0.0, 1.0);
    //FragColor = vec4(texture(screenTex, uv).rgb, 1.0);
    FragColor = texture(screenTex, uv);
}