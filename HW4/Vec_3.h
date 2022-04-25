#pragma once
#include <ostream>
template<typename T>
class Vec_3
{
public:
	T x, y, z;
	Vec_3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec_3(T xx) : x(xx), y(xx), z(xx) {}
	Vec_3(T xx, T yy, T zz) : x(xx), y(yy), z(zz){}

	Vec_3<T> operator * (const T& f) const {
		return Vec_3<T>(x * f, y * f, z * f);
	}
	Vec_3<T> operator * (const Vec_3<T>& v) const {
		return Vec_3<T>(x * v.x, y * v.y, z * v.z);
	}

	T dot(const Vec_3<T>& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	Vec_3<T> operator - (const Vec_3<T>& v) const {
		return Vec_3<T>(x - v.x, y - v.y, z - v.z);
	}
	Vec_3<T> operator + (const Vec_3<T>& v)const {
		return Vec_3<T>(x + v.x, y + v.y, z + v.z);
	}
	Vec_3<T> operator +=(const Vec_3<T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vec_3<T>& operator *=(const Vec_3<T>& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	Vec_3<T> operator - ()const {
		return Vec_3<T>(-x, -y, -z);
	}
	T length2()const {
		return x * x + y * y + z * z;
	}
	T length()const {
		return sqrt(length2());
	}
	Vec_3& normal() {
		T nor2 = length2();
		if (nor2 > 0) {
			T nor2_inv = 1 / sqrt(nor2);
			x *= nor2_inv;
			y *= nor2_inv;
			z *= nor2_inv;
		}
		return *this;
	}
	friend std::ostream& operator<<(std::ostream& os, const Vec_3<T>& v) {
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec_3<float> Vec_3f;
extern Vec_3f n_x, n_y, n_z;
