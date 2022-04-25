
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Vec_3.h"
#include "Sphere.h"
#include "Cube.h"

#define MAX_DEPTH 5

using namespace std;

Vec_3f n_x(1, 0, 0), n_y(0, 1, 0), n_z(0, 0, 1);

Vec_3f trace(
    const Vec_3f& rayorigin,
    const Vec_3f& raydirection,
    const std::vector<Sphere>& spheres,
    const std::vector<Cube>& cubes,
    const int& depth
)
{
    float tnear = INFINITY;
    const Sphere* sphere = NULL;
    const Cube* cube = NULL;
    for (unsigned i = 0; i < spheres.size(); i++)
    {
        float t0 = INFINITY;
        float t1 = INFINITY;
        if (spheres[i].intersect(rayorigin, raydirection, t0, t1))
        {
            if (t0 < 0) t0 = t1;
            if (t0 < 0) continue;
            if (t0 < tnear)
            {
                tnear = t0;
                sphere = &spheres[i];
            }
        }
    }
    for (unsigned i = 0; i < cubes.size(); i++)
    {
        float t0 = INFINITY;
        float t1 = INFINITY;
        if (cubes[i].intersect(rayorigin, raydirection, t0, t1))
        {
            if (t0 <= 0) t0 = t1;
            if (t0 <= 0) continue;
            if (t0 < tnear)
            {
                if (sphere)
                    sphere = NULL;
                tnear = t0;
                cube = &cubes[i];
            }
        }
    }
    if (!sphere && !cube) return Vec_3f(2);
    Vec_3f surfaceColor(0.02, 0.02, 0.02);
    Vec_3f phit = rayorigin + raydirection * tnear;
    Vec_3f nhit = sphere ? phit - sphere->center : (*cube).Cubenhit(phit);
    nhit.normal();
    float bias = 1e-4;
    bool inside = false;
    if (raydirection.dot(nhit) > 0)
    {
        nhit = -nhit;
        inside = true;
    }
    if (((sphere && (sphere->transparency > 0 || sphere->reflection > 0)) || (cube && (cube->transparency > 0 || cube->reflection > 0))) && depth < MAX_DEPTH) {
        if (sphere)
        {
            float facingratio = -raydirection.dot(nhit);
            float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 0.1;
            Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
            reflect_direction.normal();
            Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
            Vec_3f next_refraction = 0;
            if (sphere && sphere->transparency)
            {
                float ior = 1.1, eta = (inside) ? ior : 1 / ior;
                float cosi = -nhit.dot(raydirection);
                float k = 1 - eta * eta * (1 - cosi * cosi);
                Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
                refraction_direction.normal();
                next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
            }
            Vec_3f sphere_color = sphere->has_texture ? (*sphere).get_tex_color(phit) : sphere->surfaceColor;
            surfaceColor += (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * sphere->transparency) * sphere_color;
        }
        else {
            float facingratio = -raydirection.dot(nhit);
            float fresneleffect = pow(1 - facingratio, 3) * 0.9 + 0.1;
            Vec_3f reflect_direction = raydirection - nhit * 2 * raydirection.dot(nhit);
            reflect_direction.normal();
            Vec_3f next_reflection = trace(phit + nhit * bias, reflect_direction, spheres, cubes, depth + 1);
            Vec_3f next_refraction = 0;
            if (cube && cube->transparency)
            {
                float ior = 1.1, eta = (inside) ? ior : 1 / ior;
                float cosi = -nhit.dot(raydirection);
                float k = 1 - eta * eta * (1 - cosi * cosi);
                Vec_3f refraction_direction = raydirection * eta + nhit * (eta * cosi - sqrt(k));
                refraction_direction.normal();
                next_refraction = trace(phit - nhit * bias, refraction_direction, spheres, cubes, depth + 1);
            }
            surfaceColor += (next_reflection * fresneleffect + next_refraction * (1 - fresneleffect) * cube->transparency) * cube->surfaceColor;
        }
    }
    else {
        if (sphere) {
            for (unsigned i = 0; i < spheres.size(); i++)
            {
                if (spheres[i].emissionColor.x > 0)
                {
                    Vec_3f transmission = 1;
                    Vec_3f lightDirection = spheres[i].center - phit;
                    lightDirection.normal();
                    for (unsigned j = 0; j < spheres.size(); j++)
                    {
                        if (i != j)
                        {
                            float t0, t1;
                            if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1))
                            {
                                transmission = 0.2;
                                break;
                            }
                        }
                    }
                    Vec_3f sphere_color = sphere->has_texture ? (*sphere).get_tex_color(phit) : sphere->surfaceColor;
                    surfaceColor += sphere_color * transmission * std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor;
                }
            }
        }
        else if (cube) {
            for (unsigned i = 0; i < cubes.size(); i++)
            {
                if (cubes[i].emissionColor.x > 0)
                {
                    Vec_3f transmission = 1;
                    Vec_3f lightDirection = -nhit;
                    lightDirection.normal();
                    for (unsigned j = 0; j < cubes.size(); j++)
                    {
                        if (i != j)
                        {
                            float t0, t1;
                            if (cubes[j].intersect(phit + nhit * bias, lightDirection, t0, t1))
                            {
                                transmission = 0.2;
                                break;
                            }
                        }
                    }
                    surfaceColor += cube->surfaceColor * transmission * std::max(float(0), nhit.dot(lightDirection)) * cubes[i].emissionColor;
                }
            }
        }
    }
    return surfaceColor + (sphere ? sphere->emissionColor : cube->emissionColor);
}

void render(const std::vector<Sphere>& spheres, const std::vector<Cube>& cubes, const Vec_3f& start_ori, const float rotate, const char* filename)
{
    unsigned width = 1920, height = 1080;
    Vec_3f* img = new Vec_3f[width * height], * pixel = img;
    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 30;
    float aspectratio = width / float(height);
    float angle = tan(M_PI * 0.5 * fov / 180.0);
    unsigned int i = 0;
    clock_t a, b;
    a = time(NULL);
    cout << "Target image size: " << width << "x" << height << endl;
    for (unsigned y = 0; y < height; y++)
    {
        for (unsigned x = 0; x < width; x++, pixel++, i++)
        {
            printf("\rCalculating pixels: %8u/%8d", i + 1, width * height);
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Vec_3f raydir(xx, yy, -1);
            //rotate
            float x_old = raydir.x;
            float z_old = raydir.z;
            raydir.x = x_old * cos(rotate) + z_old * sin(rotate);
            raydir.z = -x_old * sin(rotate) + z_old * cos(rotate);
            raydir.normal();
            *pixel = trace(start_ori, raydir, spheres, cubes, 0);
        }
    }
    b = time(NULL);
    float cputime = (float)(b - a);
    cout << "\nCputime = " << cputime << endl;

    cout << "Writing file..." << endl;
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (unsigned i = 0; i < width * height; i++)
    {
        ofs << (unsigned char)(std::min(float(1), img[i].x) * 255) <<
            (unsigned char)(std::min(float(1), img[i].y) * 255) <<
            (unsigned char)(std::min(float(1), img[i].z) * 255);
    }
    ofs.close();
    delete[] img;
}

int main()
{
    vector<Sphere> spheres;
    vector<Cube> cubes;

    spheres.push_back(Sphere(Vec_3f(0.0, -10004, -20), 10000, Vec_3f(0.10, 0.10, 0.10), 0.0, 0.0));

    spheres.push_back(Sphere(Vec_3f(0.0, 0, -20), 4, Vec_3f(1.00, 0.32, 0.36), 0.5, 0.6));
    spheres.push_back(Sphere(Vec_3f(5.0, -1, -15), 2, Vec_3f(0.90, 0.76, 0.46), 1, 0.0));
    spheres.push_back(Sphere(Vec_3f(5.0, 0, -25), 3, Vec_3f(0.65, 0.77, 0.97), 1, 0.5));
    //spheres.push_back(Sphere(Vec_3f(-5.5, -1.0, -15), 2.5, Vec_3f(0.70, 0.70, 0.70), 1, 0.2));
    spheres.push_back(Sphere(Vec_3f(2.0, -1.5, -12), 0.7, Vec_3f(0.90, 0.90, 0.90), 1, 0.0));

    Sphere earth(Vec_3f(-5.5, -1.0, -15), 3, Vec_3f(0.00, 0.00, 0.00), 1, 0.2, 0, 1);
    earth.texture = stbi_load("C:\\Users\\ZYC\\Desktop\\lab4\\test\\earth.jpg", &earth.tex_width, &earth.tex_height, &earth.components_per_pixel, earth.components_per_pixel);
    spheres.push_back(earth);

    cubes.push_back(Cube(Vec_3f(-2.2, -2.7, -12), 1.0, 1.0, 1.0, Vec_3f(0.34, 1.00, 0.36), 1, 0.7));
    cubes.push_back(Cube(Vec_3f(6.3, 1.7, -20), 2.4, 2.4, 2.4, Vec_3f(0.02, 0.60, 0.95), 1, 0.8));
    cubes.push_back(Cube(Vec_3f(-6.0, -3, -23), 2.0, 15.0, 4.0, Vec_3f(0.10, 0.78, 0.78), 1, 0.8));

    spheres.push_back(Sphere(Vec_3f(0.0, 20, -10), 3, Vec_3f(0.0, 0.0, 0.0), 0, 0.0, Vec_3f(1)));
    cout << "Rendering..." << endl;
    string f_name;
    int maxframe = 300;
    for (int i = 1; i <= maxframe; i++) {
        f_name = to_string(i);
        f_name = "C:\\Users\\ZYC\\Desktop\\lab4\\test\\" + f_name + ".ppm";
        float angle = 2 * M_PI / maxframe * (i - 1);
        cout << "Frame " << i << " / " << maxframe << endl;
        render(spheres, cubes, Vec_3f(20.0 * sin(angle), 0, 20.0 * cos(angle) - 20), angle, f_name.data());
    }
    cout << "Done" << endl;
    return 0;
}
