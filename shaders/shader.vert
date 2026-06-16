#version 460 core

layout (location = 0) in vec2 pos;

out vec2 uv;

void main()
{
    uv = pos * 0.5f + 0.5f;
    gl_Position = vec4(pos, 0.0f, 1.0f);
}
