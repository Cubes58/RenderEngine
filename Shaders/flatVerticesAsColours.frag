#version 410

in vec3 colour;
out vec4 frag_colour;

void main() {

// generate colours with components between 0 and 1 for coords in (-1 ... +1)

	frag_colour = vec4 ((1.0 + colour.x) / 2, (1.0 + colour.y) / 2, (1.0 + colour.z) / 2, 1.0);
}
