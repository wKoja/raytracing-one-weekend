#include "Camera.h"
#include "Color.h"
#include "HittableList.h"
#include "Materal.h"
#include "MovingSphere.h"
#include "RTWeekend.h"
#include "Sphere.h"
#include "Vec3.h"

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <type_traits>
#include <vector>

Color ray_color(const Ray &r, const Hittable &world, int depth) {
  HitRecord rec;

  // if we've exceeded the Ray bounce limit, no more light is gathered.
  // this avoids stack overflow from the recursion
  if (depth <= 0) {
    return Color(0, 0, 0);
  }
  if (world.hit(r, 0.001, infiniy, rec)) {
    Ray scattered;
    Color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return Color(0, 0, 0);
  }
  Vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
};

HittableList random_scene() {
  HittableList world;

  auto ground_material = std::make_shared<lambertian>(Color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - Point3(4, 0.2, 0)).lenth() > 0.9) {
        shared_ptr<Materal> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = Color::random() * Color::random();
          sphere_material = make_shared<lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));

          // moving the spheres during image render
          auto center2 = center + Vec3(0, random_double(0, .5), 0);
          world.add(make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2,
                                              sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(Color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  return world;
}

int main() {
  // Image
  auto aspect_ratio = 16.0 / 9.0;
  int image_width = 400;
  int samples_per_pixel = 50;
  const int max_depth = 50;

  // World
  auto world = random_scene();

  // Camera
  Point3 lookfrom(13, 2, 3);
  Point3 lookat(0, 0, 0);
  Vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  int image_height = static_cast<int>(image_width / aspect_ratio);

  Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus,
             0.0, 1.0);

  // Render

  std::vector<Color> samples(samples_per_pixel);
  std::vector<int> idxs(samples_per_pixel);
  std::iota(idxs.begin(), idxs.end(), 0);

  int i, j;
  std::ofstream of;
  of.open("imgoutput");

  of << "P3 \n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      Color pixel_color(0, 0, 0);
      std::for_each(std::execution::par_unseq, idxs.begin(), idxs.end(),
                    [i, j, image_height, image_width, cam, &world, max_depth,
                     &samples](auto &&s) {
                      auto u = (i + random_double()) / (image_width - 1);
                      auto v = (j + random_double()) / (image_height - 1);
                      Ray r = cam.get_ray(u, v);
                      samples[s] = ray_color(r, world, max_depth);
                    });
      write_color(
          of, std::accumulate(samples.begin(), samples.end(), Color(0, 0, 0)),
          samples_per_pixel);
      /*
for (int s = 0; s < samples_per_pixel; ++s) {
auto u = (i + random_double()) / (image_width - 1);
auto v = (j + random_double()) / (image_height - 1);
Ray r = cam.get_ray(u, v);
pixel_color += ray_color(r, world, max_depth);
}
write_color(of, pixel_color, samples_per_pixel);
      */
    }
  }

  std::cerr << "\nDone.\n";
  of.close();
  return 0;
}
