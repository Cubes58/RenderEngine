#version 410

in vec3 position_eye, normal_eye;

uniform mat4 view_matrix;
uniform vec3 surface_colour;

uniform vec3 light_position_world;

//--uniform vec3 light_colour_specular;
uniform vec3 light_colour_diffuse;
uniform vec3 light_colour_ambient;

  
// surface reflectance (diffuse is passed as a uniform)
//--vec3 surface_specular = vec3 (1.0, 1.0, 1.0); // fully reflect specular light
vec3 surface_ambient = vec3 (1.0, 1.0, 1.0); // fully reflect ambient light


//--float specular_exponent = 100.0; // specular 'power'

out vec4 fragment_colour; // final colour of surface

void main () {
	// **ambient intensity**
	vec3 Ia = light_colour_ambient * surface_colour * surface_ambient;

	// **diffuse intensity**
	// raise light position to eye space
	vec3 light_position_eye = vec3 (view_matrix * vec4 (light_position_world, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	float dot_prod = dot (direction_to_light_eye, normal_eye);
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = light_colour_diffuse * surface_colour * dot_prod; // final diffuse intensity WAS Kd
	
	// **specular intensity**
	//---vec3 surface_to_viewer_eye = normalize (-position_eye);
	
	// alternative method
	//vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);
	//float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
	//dot_prod_specular = max (dot_prod_specular, 0.0);
	//float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	// blinn
	//--vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);
	//--float dot_prod_specular = max (dot (half_way_eye, normal_eye), 0.0);
	//--float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	//--vec3 Is = light_colour_specular * surface_specular * specular_factor; // final specular intensity
	
	// final colour
	fragment_colour = vec4 (Id + Ia, 1.0);
}
