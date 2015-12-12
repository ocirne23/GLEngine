#include "Shaders/globals.glsl"
#include "Shaders/material.glsl"

////////////////////////// IN / OUT //////////////////////////

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
flat in uint v_materialID;

layout (location = 0) out vec3 out_color;

vec3 calcExtinction(float dist) {
  return exp(dist * sky_params6.xyz);
}

vec3 calcScattering(float cos_theta) {
  float r_phase = (cos_theta * cos_theta) * sky_params6.w + sky_params6.w;
  float m_phase = sky_params1.w * pow(sky_params2.w * cos_theta + sky_params3.w, -1.5);
  return sky_params2.xyz * r_phase + (sky_params3.xyz * m_phase);
}

float baseOpticalDepth(in vec3 ray) {
  float a1 = sky_params4.x * ray.z;
  return sqrt(a1 * a1 + sky_params4.w) - a1;
}

float opticalDepth(in vec3 pos, in vec3 ray) {
  pos.z += sky_params4.x;
  float a0 = sky_params4.y - dot(pos, pos);
  float a1 = dot(pos, ray);
  return sqrt(a1 * a1 + a0) - a1;
}

void main()
{
	float cos_theta = dot(view_vec, sun_vector);

	// optical depth along view ray
	float ray_dist = baseOpticalDepth(view_vec);

	// extinction of light along view ray
	vec3 extinction = calcExtinction(ray_dist);

	// optical depth for incoming light hitting the view ray
	vec3 light_ray_pos = view_vec * (ray_dist * sky_params4.z);
	float light_ray_dist = opticalDepth(light_ray_pos, sun_vector);

	// optical depth for edge of atmosphere:
	// this handles the case where the sun is low in the sky and
	// the view is facing away from the sun; in this case the distance
	// the light needs to travel is much greater
	float light_ray_dist_full = opticalDepth(view_vec * ray_dist, sun_vector);

	light_ray_dist = max(light_ray_dist, light_ray_dist_full);

	// cast a ray towards the sun and calculate the incoming extincted light
	vec3 incoming_light = calcExtinction((light_ray_dist);

	// calculate the in-scattering
	vec3 scattering = calcScattering(cos_theta);
	scattering *= 1.0 - extinction;

	// combine
	vec3 in_scatter = incoming_light * scattering;

	// sun disk
	float sun_strength = clamp(cos_theta * sky_params1.x + sky_params1.y, 0.0, 1.0);
	sun_strength *= sun_strength;
	vec3 sun_disk = extinction * sun_strength;

	out_color = sky_params5.xyz * (sky_params5.w * sun_disk + in_scatter);
}

