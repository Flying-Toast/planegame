#version 320 es
precision highp float;

out vec4 color;

in vec3 fragnorm;
in vec2 fraguv;

void main() {
	color = vec4(1.0, 0.4, 0.2, 1.0);
	color = vec4(fraguv,fragnorm.x, 1.);
}
