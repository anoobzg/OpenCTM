cmake_minimum_required(VERSION 3.15)
project(vtkOpenCTM)

INCLUDE_DIRECTORIES(lib)
INCLUDE_DIRECTORIES(lib/liblzma)
INCLUDE_DIRECTORIES(tools/rply)
INCLUDE_DIRECTORIES(tools/tinyxml)

AUX_SOURCE_DIRECTORY(lib ctm)
AUX_SOURCE_DIRECTORY(lib/liblzma lzma)

if(BUILD_TOOLS)
	AUX_SOURCE_DIRECTORY(tools/rply rply)
	AUX_SOURCE_DIRECTORY(tools/tinyxml tinyxml)
endif()

if(BUILD_VTK)
	FIND_PACKAGE(VTK REQUIRED)
	INCLUDE(${VTK_USE_FILE})

	ADD_DEFINITIONS (-D VTKINCLUDED)

	ADD_EXECUTABLE(mesh2ctm
		tools/ctmconv.cpp
		tools/convoptions.cpp
		tools/systimer.cpp
		tools/mesh.cpp
		tools/meshio.cpp
		tools/ctm.cpp
		tools/meshio.cpp
		tools/ply.cpp
		tools/stl.cpp
		tools/3ds.cpp
		tools/dae.cpp
		tools/lwo.cpp
		tools/obj.cpp
		tools/off.cpp
		tools/wrl.cpp
		tools/common.cpp
		${ctm} ${lzma} ${rply} ${tinyxml}
	)
	TARGET_LINK_LIBRARIES(mesh2ctm ${VTK_LIBRARIES})
endif()

