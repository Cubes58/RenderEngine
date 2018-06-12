/* NOTE: this shader is for GLSL 4.2.0 (OpenGL 4.2)
 to convert it to an earlier version, for example on Apple, you'll need to
 remove layout (binding = x) for each texture, and instead explicitly
 set glUniform1i() for each texture in C with these values */

#version 410
// this picks up the data in the vertex array by index
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_textureUV;

// we pass the view matrix from the camera
// the projection matrix (which remains constant) and the
// model matrix for the position of each model
uniform mat4 view_matrix, projection_matrix, model_matrix;

// lighting calculations are simplest in "eye space" that is relative to the eye's position
// we calculate these values in the vertex shader and pass them to the fragment shader
// note these will be interpolated (using perspective) 
out vec3 position_eye;		// position in eye space
out vec3 normal_eye; 		// normals in eye space
out vec2 texture_coordinates; // texture coordinates passed through from from vertex shader

void main() {
	texture_coordinates = vertex_textureUV;
	
	// working in eye space coordinates - so you are at 0,0,0.
	// model is at (view_matrix * model_matrix)
	// we map this via projection to screen
	
	// for the normal we only want the rotations, not translations so we use a vec4 with w term == 0

	// adjust the normal matrix to compensate for scaling
	mat4 normalMatrix = transpose(inverse(view_matrix * model_matrix));
	
	normal_eye = normalize(vec3 (normalMatrix * vec4 (vertex_normal, 0.0)));   // ignore w term!!! - rotations only!!!
	position_eye = vec3 (view_matrix * model_matrix * vec4 (vertex_position, 1.0));  // eye at 0,0,0 in eye coords... this will vary for fragment!!
	
	// this is the standard calculation of the vertex position
	// we would usually  put projection_matrix * view_matrix * model_matrix * vertex_position 
	// but we've already calculated view_matrix * model_matrix * vertex_position in position_eye
	
	
	
	gl_Position = projection_matrix * vec4 (position_eye, 1.0);
}
