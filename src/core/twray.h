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
#include "samplers/halton.h"
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
#include "fileutil.h"

#include "stats.h"
#include "parallel.h"
#include "qt/qt.h"

#include <iostream>
#include <map>

namespace pbrt{

Medium* MakeMedium(const ParamSet &paramSet);
Medium* addMedium(std::string name, Vector3f sigma_a, 
            Vector3f sigma_s, float g, 
            float scale);
std::shared_ptr<Material> addGlassMat();
std::shared_ptr<Material> addMatteMat(Vector3f color);
std::shared_ptr<Material> addDisneyMat(Vector3f color, float metallic);
std::shared_ptr<Material> addSubsurfaceMat(Vector3f color, std::string name, float scale, float roughness);
std::shared_ptr<Material> addUberMat(Vector3f Kd, Vector3f Ks, float roughness, float index);
std::shared_ptr<Shape> addSphereShape(Vector3f pos, float radius);
std::shared_ptr<Primitive> addBasicDisk(Vector3f pos, float radius, MediumInterface mi);
std::vector<std::shared_ptr<Shape>> addPlaneShape(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale);
std::vector<std::shared_ptr<Primitive>> addPlanePrim(Vector3f pos, Vector3f rot, 
                                                        Vector3f scale, Vector3f color,
                                                        std::shared_ptr<AreaLight> area,
                                                        MediumInterface mi);
std::shared_ptr<Light> addPointLight(Vector3f pos, Vector3f intensity, MediumInterface &mi);
std::shared_ptr<Light> addSpotLight(Vector3f pos, Vector3f intensity, Vector3f to, float coneangle, MediumInterface &mi);
std::shared_ptr<Light> addDistantLight(Point3f dir, float intensity);
std::shared_ptr<AreaLight> addAreaLight(std::shared_ptr<Shape> shape, Vector3f pos, 
                                            Vector3f intensity, MediumInterface &mi);
std::shared_ptr<Light> addInfiniteLight(std::string filename, Vector3f intensity, MediumInterface &mi);
std::shared_ptr<const Camera> addCamera(Point3f origin, Point3f lookAt, Vector3f up, 
                                        float fovc, int image_width, int image_height, 
                                        MediumInterface mi, std::string filename);
std::vector<std::shared_ptr<Primitive>> addStanfordBunny(Vector3f pos, float color[3], MediumInterface mi);
std::vector<std::shared_ptr<Primitive>> addStanfordDragon(Vector3f pos, float color[3], MediumInterface mi);                                                                            
std::vector<std::shared_ptr<Primitive>> addGlassBottle(Vector3f pos, float color[3], MediumInterface mi);
std::vector<std::shared_ptr<Primitive>> addCausticsPlane(MediumInterface mi);
void addPoly(std::string path, Vector3f pos, std::shared_ptr<Material> material, MediumInterface mi,
                std::vector<std::shared_ptr<Primitive>> &objects,
                std::vector<std::shared_ptr<Light>> &lights);
void addCornellBox(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);
void addSampleScene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);  
void addCausticsScene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);    
void addWineGlassScene(std::vector<std::shared_ptr<Primitive>> &objects,
                    std::vector<std::shared_ptr<Light>> &lights,
                    float intensity,
                    MediumInterface mi);
              
}

#endif