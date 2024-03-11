#version 330 core

uniform sampler2D s_texture;

in vec2 texCoord;
in vec3 normal;
flat in int layerNumber;

out vec4 fragColor;

void main()
{

    vec4 tex = texture2D(s_texture, texCoord);

    float threshold = 1.0 / 45.0;

    float currentThreshold = threshold * layerNumber;
    
    if(tex.r > currentThreshold) {
        fragColor.rgb = vec3(160.0 / 255, 82.0 / 255,45.0 / 255) * (layerNumber / 45.0);
        fragColor.a = 1;
    } else {
        discard;
    }

}