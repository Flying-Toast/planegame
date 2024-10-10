#version 320 es
precision highp float;

in vec3 pos;
in vec3 norm;
in vec2 st;

out vec3 fragnorm;
out vec2 fragst;

uniform mat4 transform;

void main() {
	fragnorm = norm;
	fragst = st;
	gl_Position = transform * vec4(pos, 1.0);
}
