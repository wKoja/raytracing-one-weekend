#ifndef MATERIAL_H
#define MATERIAL_H

#include "Hittable.h"
#include "RTWeekend.h"

struct HitRecord;

class Materal {
public:
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const = 0;
};

class lambertian : public Materal {
public:
  lambertian(const Color &a) : albedo(a) {}

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const override {
    auto scattered_direction = rec.normal + random_unit_vector();

    // catch degenerate scatter direction
    if (scattered_direction.near_zero())
      scattered_direction = rec.normal;

    scattered = Ray(rec.p, scattered_direction, r_in.time());
    attenuation = albedo;
    return true;
  }

public:
  Color albedo;
};

class metal : public Materal {
public:
  metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const override {

    Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered =
        Ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  Color albedo;
  double fuzz;
};

class dielectric : public Materal {
public:
  dielectric(double index_of_refraction) : ir(index_of_refraction) {}
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const override {
    attenuation = Color(1.0, 1.0, 1.0);
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    Vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > random_double())
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, refraction_ratio);

    scattered = Ray(rec.p, direction, r_in.time());

    return true;
  }

public:
  double ir; // index of refraction

private:
  static double reflectance(double cosine, double ref_idx) {
    // use Schlick's approximation for reflectance
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

#endif
