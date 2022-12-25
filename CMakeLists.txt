cmake_minimum_required(VERSION 3.10)

project(RayTracing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -fansi-escape-codes")

find_package(glog REQUIRED)

include_directories(
  /usr/local/include/
  /usr/local/Cellar/glog/0.5.0/include/
  /usr/local/Cellar/openexr/3.1.3/include/OpenEXR/
  /usr/local/Cellar/imath/3.1.3/include/Imath/
  /usr/local/Cellar/gflags/2.2.2/include/
)

link_directories(
  /usr/local/lib/
  /usr/local/Cellar/glog/0.5.0/lib/
  /usr/local/Cellar/openexr/3.1.3/lib/
  /usr/local/Cellar/imath/3.1.3/lib/
  /usr/local/Cellar/gflags/2.2.2/lib/
)

add_library(pbrt STATIC
  bssrdf.cpp
  bvh.cpp
  camera.cpp
  cameras/perspective.cpp
  efloat.cpp
  error.cpp
  ext/rply.cpp
  ext/lodepng.cpp
  ext/targa.cpp
  imageio.cpp
  interaction.cpp
  interpolation.cpp
  integrator.cpp
  integrators/directlighting.cpp
  integrators/path.cpp
  integrators/volpath.cpp
  integrators/bdpt.cpp
  fileutil.cpp
  film.cpp
  filter.cpp
  filters/box.cpp
  floatfile.cpp
  geometry.cpp
  light.cpp
  lightdistrib.cpp
  lights/diffuse.cpp
  lights/distant.cpp
  lights/point.cpp
  lights/spot.cpp
  lowdiscrepancy.cpp
  material.cpp
  materials/matte.cpp
  materials/glass.cpp
  materials/disney.cpp
  materials/subsurface.cpp
  media/homogeneous.cpp
  medium.cpp
  memory.cpp
  microfacet.cpp
  parallel.cpp
  paramset.cpp
  primitive.cpp
  progressreporter.cpp
  quaternion.cpp
  reflection.cpp
  sampling.cpp
  sampler.cpp
  samplers/random.cpp
  samplers/zerotwosequence.cpp
  scene.cpp
  shape.cpp
  shapes/sphere.cpp
  shapes/disk.cpp
  shapes/plymesh.cpp
  shapes/triangle.cpp
  shapes/heightfield.cpp
  sobolmatrices.cpp
  spectrum.cpp
  stats.cpp
  texture.cpp
  textures/constant.cpp
  transform.cpp
)

SET(ALL_PBRT_LIBS
  pbrt
  gflags
  glog
  openexr
  Imath
)

add_executable(RayTracing
  main.cc
)

target_link_libraries(RayTracing
  ${ALL_PBRT_LIBS}
  # /usr/local/include/OpenEXR/
  # /usr/local/include/Imath/
  # /usr/local/Cellar/ilmbase/
  # /usr/local/Cellar/glog/0.5.0/include
  # /usr/local/Cellar/gflags/2.2.2/include
)