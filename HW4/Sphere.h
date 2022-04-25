#pragma once
#include "Vec_3.h"
using namespace std;
class Sphere
{
public:
	Vec_3f center;
	float radius, radius2;
	Vec_3f surfaceColor, emissionColor;
	float transparency, reflection;
	bool has_texture;
	unsigned char* texture;
	int tex_width;
	int tex_height;
	int components_per_pixel;
	Sphere(
		const Vec_3f& c,
		const float& r,
		const Vec_3f& sc,
		const float& refl = 0,
		const float &transp = 0,
		const Vec_3f &ec = 0,
		const bool use_texture = 0) :
		center(c), radius(r), radius2(r*r), surfaceColor(sc), emissionColor(ec),
		transparency(transp), reflection(refl), has_texture(use_texture), texture(NULL),
		tex_width(0), tex_height(0), components_per_pixel(3)
	{}
	bool intersect(const Vec_3f& rayorigin, const Vec_3f& raydirection, float& t0, float& t1) const {
		Vec_3f l = center - rayorigin;
		float tca = l.dot(raydirection);
		if (tca < 0) return false;
		float dist = l.dot(l) - tca * tca;
		if (dist > radius2) return false;
		float thc = sqrt(radius2 - dist);
		t0 = tca - thc;
		t1 = tca + thc;
		return true;
	}
	Vec_3f get_tex_color (Vec_3f p) const {
		Vec_3f r = p - center;
		r.normal();
		//cout << r.x << " " << r.y << " " << r.z << ". ";
		float theta = acos(-r.y);
		float phi = atan2(-r.z, r.x) + M_PI;
		float u = phi / (2 * M_PI);
		float v = theta / M_PI;
		v = 1 - v;
		int i = u * tex_width;
		int j = v * tex_height;
		if (i >= tex_width) 
			i = tex_width - 2;
		if (j >= tex_height)
			j = tex_height - 2;
		unsigned char * pixel = texture + j * components_per_pixel * tex_width + i * components_per_pixel;
		Vec_3f pixel_color((float)pixel[0] / 255.0, (float)pixel[1] / 255.0, (float)pixel[2] / 255.0);
		return pixel_color;
	}
};

