#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Vec3.h"
#include <math.h>

class Sphere : public Hittable {
public:
  Sphere() {}
  Sphere(Point3 cen, double r, shared_ptr<Materal> m)
      : center(cen), radius(r), mat_ptr(m){};

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

public:
  Point3 center;
  double radius;
  shared_ptr<Materal> mat_ptr;
};

bool Sphere::hit(const Ray &r, double t_min, double t_max,
                 HitRecord &rec) const {
  Vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;

  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;
  auto sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in acceptable range
  auto root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  Vec3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}

#endif
