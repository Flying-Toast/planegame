#version 320 es
precision highp float;

in vec3 pos;
in vec3 norm;
in vec2 uv;

out vec3 fragnorm;
out vec2 fraguv;

void main() {
	fragnorm = norm;
	fraguv = uv;
	gl_Position = vec4(pos, 1.0);
}
