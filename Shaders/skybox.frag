#version 410

in vec3 texture_coordinates;

out vec4 fragment_colour;

uniform samplerCube skybox;

void main() {
	fragment_colour = texture(skybox, texture_coordinates);
}
