#ifndef TWRAY_H
#define TWRAY_H

#include "pbrt.h"
#include "spectrum.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include "texture.h"

#include "paramset.h"
#include "primitive.h"
#include "camera.h"
#include "bvh.h"
#include "cameras/perspective.h"
#include "lights/point.h"
#include "lights/diffuse.h"
#include "lights/distant.h"
#include "lights/spot.h"
#include "lights/infinite.h"
#include "samplers/random.h"
#include "samplers/zerotwosequence.h"
#include "shapes/sphere.h"
#include "shapes/plymesh.h"
#include "shapes/disk.h"
#include "shapes/heightfield.h"

#include "materials/matte.h"
#include "materials/glass.h"
#include "materials/disney.h"
#include "materials/subsurface.h"
#include "materials/uber.h"
#include "media/homogeneous.h"
#include "integrators/directlighting.h"
#include "integrators/path.h"
#include "integrators/volpath.h"
#include "integrators/bdpt.h"
#include "integrators/sppm.h"
#include "scene.h"
#include "filters/box.h"

#include "stats.h"
#include "parallel.h"

#include <iostream>
#include <map>

namespace pbrt{

Medium* MakeMedium(const ParamSet &paramSet);
Medium* add_medium(std::string name, Vector3f sigma_a, 
            Vector3f sigma_s, float g, 
            float scale);
std::shared_ptr<Material> add_glass_mat();
std::shared_ptr<Material> add_matte_mat(Vector3f color);
std::shared_ptr<Material> add_disney_mat(Vector3f color, float metallic);
std::shared_ptr<Material> add_subsurface_mat(Vector3f color, std::string name, float scale);
std::shared_ptr<Material> add_uber_mat(Vector3f Kd, Vector3f Ks, float roughness, float index);
std::shared_ptr<Shape> add_sphere_shape(Vector3f pos, float radius);
std::shared_ptr<Primitive> add_basic_disk(Vector3f pos, float radius, MediumInterface mi);
std::vector<std::shared_ptr<Shape>> add_plane_shape(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale);
std::vector<std::shared_ptr<Primitive>> add_plane_prim(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale, Vector3f color,
                                                        std::shared_ptr<AreaLight> area,
                                                        MediumInterface mi);
std::shared_ptr<Light> add_point_light(Vector3f pos, Vector3f intensity, MediumInterface &mi);
std::shared_ptr<Light> add_spot_light(Vector3f pos, Vector3f intensity, Vector3f to, float coneangle, MediumInterface &mi);
std::shared_ptr<Light> add_distant_light(Point3f dir, float intensity);
std::shared_ptr<AreaLight> add_area_light(std::shared_ptr<Shape> shape, Vector3f pos, 
                                            Vector3f intensity, MediumInterface &mi);
std::shared_ptr<Light> add_infinite_light(std::string filename, Vector3f intensity, MediumInterface &mi);
std::shared_ptr<const Camera> add_camera(Point3f origin, Point3f lookAt, Vector3f up, 
                                        float fovc, int image_width, int image_height, 
                                        MediumInterface mi);
std::vector<std::shared_ptr<Primitive>> add_stanford_bunny(Vector3f pos, float color[3], MediumInterface mi);
std::vector<std::shared_ptr<Primitive>> add_stanford_dragon(Vector3f pos, float color[3], MediumInterface mi);                                                                            
std::vector<std::shared_ptr<Primitive>> add_glass_bottle(Vector3f pos, float color[3], MediumInterface mi);
std::vector<std::shared_ptr<Primitive>> add_caustics_plane(MediumInterface mi);
void add_cornell_box(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);
void add_sample_scene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);  
void add_caustics_scene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);                  
}

#endif