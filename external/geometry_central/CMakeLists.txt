file(GLOB geosrc include/geometry_central/*.h include/geometry_central/*.ipp src/*.cpp)
add_library(geometry_central ${geosrc})