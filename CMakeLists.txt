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
  src
  src/core
)

link_directories(
  /usr/local/lib/
  /usr/local/Cellar/glog/0.5.0/lib/
  /usr/local/Cellar/openexr/3.1.3/lib/
  /usr/local/Cellar/imath/3.1.3/lib/
  /usr/local/Cellar/gflags/2.2.2/lib/
)

add_library(pbrt STATIC
  src/core/bssrdf.cpp
  src/core/bvh.cpp
  src/core/camera.cpp
  src/core/efloat.cpp
  src/core/error.cpp
  src/core/imageio.cpp
  src/core/interaction.cpp
  src/core/interpolation.cpp
  src/core/integrator.cpp
  src/core/fileutil.cpp
  src/core/film.cpp
  src/core/filter.cpp
  src/core/floatfile.cpp
  src/core/geometry.cpp
  src/core/light.cpp
  src/core/lightdistrib.cpp
  src/core/lowdiscrepancy.cpp
  src/core/material.cpp
  src/core/medium.cpp
  src/core/memory.cpp
  src/core/microfacet.cpp
  src/core/parallel.cpp
  src/core/paramset.cpp
  src/core/primitive.cpp
  src/core/progressreporter.cpp
  src/core/quaternion.cpp
  src/core/reflection.cpp
  src/core/sampling.cpp
  src/core/sampler.cpp
  src/core/scene.cpp
  src/core/shape.cpp
  src/core/sobolmatrices.cpp
  src/core/spectrum.cpp
  src/core/stats.cpp
  src/core/texture.cpp
  src/core/transform.cpp
  src/core/twray.cpp
  src/cameras/perspective.cpp
  src/ext/rply.cpp
  src/ext/lodepng.cpp
  src/ext/targa.cpp
  src/integrators/directlighting.cpp
  src/integrators/path.cpp
  src/integrators/volpath.cpp
  src/integrators/bdpt.cpp
  src/integrators/sppm.cpp
  src/filters/box.cpp
  src/lights/diffuse.cpp
  src/lights/distant.cpp
  src/lights/point.cpp
  src/lights/spot.cpp
  src/lights/infinite.cpp
  src/materials/matte.cpp
  src/materials/glass.cpp
  src/materials/disney.cpp
  src/materials/subsurface.cpp
  src/materials/uber.cpp
  src/media/homogeneous.cpp
  src/samplers/random.cpp
  src/samplers/zerotwosequence.cpp
  src/samplers/halton.cpp
  src/shapes/sphere.cpp
  src/shapes/disk.cpp
  src/shapes/plymesh.cpp
  src/shapes/triangle.cpp
  src/shapes/heightfield.cpp
  src/textures/constant.cpp
  src/textures/checkerboard.cpp
)

FILE ( GLOB PBRT_SOURCE
  src/ext/*
  src/accelerators/*
  src/cameras/*
  src/filters/*
  src/integrators/*
  src/lights/*
  src/materials/*
  src/samplers/*
  src/shapes/*
  src/textures/*
  src/media/*
)


set(ALL_PBRT_LIBS
  pbrt
  gflags
  glog
  openexr
  Imath
)

add_executable(RayTracing
  src/core/main.cc
)

target_link_libraries(RayTracing
  ${ALL_PBRT_LIBS}
)