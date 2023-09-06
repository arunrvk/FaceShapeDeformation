cmake_minimum_required (VERSION 2.8)
project (ShapeDeformation) 

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

find_package(OpenCV REQUIRED)

add_executable(ShapeDeformation 
	std.cpp std.h
		MLS.h MLS.cpp
		CurveUtility.cpp CurveUtility.h
		Curve.cpp Curve.h
	)
	
target_link_libraries(ShapeDeformation
	${OpenCV_LIBS}
	)
	
set_target_properties(ShapeDeformation PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "std.h")
set_target_properties(ShapeDeformation PROPERTIES COMPILE_FLAGS "-DWITHOUT_OPENCL")
