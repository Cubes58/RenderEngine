/* modified Antons shader to be 4.0 compatible - PC 15/9/16 */


#version 410

// passed from vertex shader
in vec3 position_eye;
in vec3 normal_eye;
in vec2 texture_coordinates;

// element 2 in vao is textures

uniform sampler2D basic_texture;

// from CPU

uniform mat4 view_matrix;

uniform vec3 light_position_world;
uniform vec3 light_colour_specular;
uniform vec3 light_colour_diffuse;
uniform vec3 light_colour_ambient;

// we add a variable to set the specular_level in range 0..1 to avoid shiny floor
uniform float specular_intensity;

out vec4 frag_colour;

// thids is specular colour - usually white for a white light
vec3	surface_specular = vec3(1.0f, 1.0f, 1.0f);

// specular 'sharpness'
uniform float specular_exponent = 30.0; 					// specular 'power'

void main() {
	
	vec4 texel = texture (basic_texture, texture_coordinates);
	
	// we assume the diffuse and ambient terms are based on the texel
	
	vec3 surface_ambient = texel.rgb;
	vec3 surface_diffuse = texel.rgb;	
	
	// ambient term
	vec3 Ia = light_colour_ambient * surface_ambient;

	// diffuse term	

	// move the light into eye space coords - this could be done in the vertex shader?
	// as light_position_world and view_matrx are not interpolated

	vec3 light_position_eye = (view_matrix * vec4 (light_position_world, 1.0)).xyz;		// NOT model_mat!!!!
	
	// construct the vector from the fragment to the light - 
	// position_eye will vary for each fragment - see vertex shader
	// so this needs to be in the fragment shader
	vec3 surface_to_light_eye = normalize (light_position_eye - position_eye);
	
	// normal_eye should already be a unit vector as a normal
	// get the cos of the angle between the light and the normal - the dot product
	
	float dot_prod = dot (normal_eye, surface_to_light_eye);
	
	// disallow negative values - map to zero
	
	dot_prod = max (0.0, dot_prod);
	
	vec3 Id = light_colour_diffuse * surface_diffuse * dot_prod; 		// the text colour is scaled by 0.7 Ld term then by this dot product

	
	// specular - assume white light and white surface for specular - can't use texel as then black has no specular value...
	// we calculate the specular_factor
	
	vec3 surface_to_viewer_eye = normalize (-position_eye);
	
	vec3 half_way_eye = normalize (surface_to_viewer_eye + surface_to_light_eye);
	
	float dot_prod_specular = max (dot (half_way_eye, normal_eye), 0.0);
	
	float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	// factor in intensity also
	specular_factor *= specular_intensity;
	
	vec3 Is = light_colour_specular * surface_specular * specular_factor; // final specular intensity

	frag_colour = vec4 (Ia + Id + Is, 1.0); 	// add ambient diffuse and specular terms
}
