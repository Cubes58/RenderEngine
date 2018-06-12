#version 410

uniform vec3 surface_colour;

out vec4 frag_colour;

void main() {
	frag_colour = vec4 (surface_colour, 1.0);
}
