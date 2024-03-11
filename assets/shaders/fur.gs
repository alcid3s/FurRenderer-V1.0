#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 1800) out;


in vec2[] texCoordVs;
in vec3[] normal;

out vec2 texCoord;
flat out int layerNumber;

void main() {
    int furLayers = 45;
    float furLayersSpaceBetween = 0.003;

    for(int layer = 0; layer < furLayers; ++layer) {
        for (int i = 0; i < 3; ++i) {
            texCoord = texCoordVs[i];
            gl_Position = gl_in[i].gl_Position + vec4( normal[i] * layer * furLayersSpaceBetween, 0.0);

            layerNumber = layer;
            EmitVertex();
        }
        EndPrimitive();
    }
}