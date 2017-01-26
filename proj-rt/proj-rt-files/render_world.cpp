#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

Render_World::Render_World() :
		background_shader(0), ambient_intensity(0), enable_shadows(true), recursion_depth_limit(
				3)
{
}

Render_World::~Render_World()
{
	delete background_shader;
	for (size_t i = 0; i < objects.size(); i++)
		delete objects[i];
	for (size_t i = 0; i < lights.size(); i++)
		delete lights[i];
}

// Find the closest object of intersection and return the object that was
// intersected.  Record the Hit structure in hit.  If no intersection occurred,
// return NULL.  Note that in the case of a Boolean, the object returned will be
// the Boolean, but the object stored in hit will be the underlying primitive.
// Any intersection with t<=small_t should be ignored.
Object* Render_World::Closest_Intersection(const Ray& ray, Hit& hit)
{
	hit.t = -1;
	Object* ret = 0;
	for (std::vector<Object*>::const_iterator cit = objects.begin();
			cit != objects.end(); ++cit)
	{
		std::vector<Hit> hits;
		if ((*cit)->Intersection(ray, hits))
		{
			for (std::vector<Hit>::const_iterator cit2 = hits.begin();
					cit2 != hits.end(); ++cit2)
			{
				if (cit2->t > small_t)
				{
					if ((cit2->t < hit.t) || (hit.t < 0))
					{
						std::cout << "hit" << std::endl;
						hit.t = cit2->t;
						hit.ray_exiting = cit2->ray_exiting;
						hit.object = cit2->object; // Only premitive.
						ret = (*cit); // Can be Boolean.
					}
					break;
				}
			}
		}
	}
	return ret;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
	double horizontal_a = (camera.max[0] - camera.min[0])
			/ camera.number_pixels[0];
	double horizontal_b = camera.min[0] + horizontal_a / 2;
	double r = horizontal_a * pixel_index[0] + horizontal_b;

	double vertical_a = (camera.max[1] - camera.min[1])
			/ camera.number_pixels[1];
	double vertical_b = camera.min[1] + vertical_a / 2;
	double s = vertical_a * pixel_index[1] + vertical_b;

	Ray ray;
	ray.endpoint = camera.position;
	vec3 z = camera.film_position + r * camera.horizontal_vector
			+ s * camera.vertical_vector;
	ray.direction = (z - ray.endpoint).normalized();
	vec3 color = Cast_Ray(ray, recursion_depth_limit);
	camera.Set_Pixel(pixel_index, Pixel_Color(color));
}

void Render_World::Render()
{
	for (int j = 0; j < camera.number_pixels[1]; j++)
		for (int i = 0; i < camera.number_pixels[0]; i++)
			Render_Pixel(ivec2(i, j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray, int recursion_depth)
{
	// TODO
	vec3 color;
	Hit hit;
	Object* o = Closest_Intersection(ray, hit);
	if (o != NULL)
	{
		vec3 intersection_point = ray.endpoint + ray.direction * hit.t;
		vec3 same_side_normal = hit.object->Normal(intersection_point);
		color = hit.object->material_shader->Shade_Surface(ray,
				intersection_point, same_side_normal, recursion_depth);
	}
	else
	{
		//std::cout<<"SKCEHC"<<std::endl;
		if (background_shader != 0)
		{
			// TODO background shader.
		}
	}
	// determine the color here

	return color;
}
