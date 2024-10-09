#version 320 es
precision highp float;

out vec4 color;

in vec3 fragnorm;
in vec2 fragst;

uniform sampler2D tex;

void main() {
	color = texture(tex, fragst + 0.00000000000001*fragnorm.x);
}
