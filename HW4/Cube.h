#pragma once
#include "Vec_3.h"
class Cube
{
public:
	Vec_3f vertex;
	float xLength, yLength, zLength;
	Vec_3f surfaceColor, emissionColor;
	float transparency, reflection;
	bool has_texture;
	unsigned char* texture;
	int tex_width;
	int tex_height;
	Cube(
		const Vec_3f& vertex,
		const float& xLength,
		const float& yLength,
		const float& zLength,
		const Vec_3f& sc,
		const float& refl = 0,
		const float& transp = 0,
		const Vec_3f& ec = 0, 
		const bool use_texture = 0) :
		vertex(vertex), xLength(xLength), yLength(yLength), zLength(zLength),
		surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl),
		has_texture(use_texture), texture(NULL), tex_width(0), tex_height(0)
	{}
	bool intersect(const Vec_3f& rayorigin, const Vec_3f& raydirection, float& t0, float& t1) const {
		float t_x1 = -(vertex.x - n_x.dot(rayorigin)) / -n_x.dot(raydirection);
		float t_x2 = -(-(vertex.x + xLength) + n_x.dot(rayorigin)) / n_x.dot(raydirection);
		float t_y1 = -(vertex.y - n_y.dot(rayorigin)) / -n_y.dot(raydirection);
		float t_y2 = -(-(vertex.y + yLength) + n_y.dot(rayorigin)) / n_y.dot(raydirection);
		float t_z1 = -(vertex.z - n_z.dot(rayorigin)) / -n_z.dot(raydirection);
		float t_z2 = -(-(vertex.z + zLength) + n_z.dot(rayorigin)) / n_z.dot(raydirection);
		float t_xmin = fmin(t_x1, t_x2);
		float t_xmax = fmax(t_x1, t_x2);
		float t_ymin = fmin(t_y1, t_y2);
		float t_ymax = fmax(t_y1, t_y2);
		float t_zmin = fmin(t_z1, t_z2);
		float t_zmax = fmax(t_z1, t_z2);
		float t_xymin = fmin(t_xmin, t_ymin);
		float t_xyzmin = fmin(t_xymin, t_zmin);
		float t_xymax = fmax(t_xmax, t_ymax);
		float t_xyzmax = fmax(t_xymax, t_zmax);
		float t_xymin_max = fmax(t_xmin, t_ymin);
		float t_xyzmin_max = fmax(t_xymin_max, t_zmin);
		float t_xymax_min = fmin(t_xmax, t_ymax);
		float t_xyzmax_min = fmin(t_xymax_min, t_zmax);
		if (t_xyzmax_min < t_xyzmin_max || t_xyzmax <= 0)
			return false;
		t0 = t_xyzmin_max;
		t1 = t_xyzmax_min;
		return true;
	}
	Vec_3f Cubenhit(const Vec_3f& phit) const {
		float dis[6] = { 0.0 };
		dis[0] = abs(phit.x - vertex.x);
		dis[1] = abs(phit.x - vertex.x - xLength);
		dis[2] = abs(phit.y - vertex.y);
		dis[3] = abs(phit.y - vertex.y - yLength);
		dis[4] = abs(phit.z - vertex.z);
		dis[5] = abs(phit.z - vertex.z - zLength);
		int min = 0;
		for (int i = 0; i < 6; i++)
		{
			if (dis[i] < dis[min])
			{
				min = i;
			}
		}
		switch (min)
		{
		case 0:
			return -n_x;
			break;
		case 1:
			return n_x;
			break;
		case 2:
			return -n_y;
			break;
		case 3:
			return n_y;
			break;
		case 4:
			return -n_z;
			break;
		case 5:
			return n_z;
			break;
		default:
			return Vec_3f(0, 0, 0);
			break;
		}

	}
};

