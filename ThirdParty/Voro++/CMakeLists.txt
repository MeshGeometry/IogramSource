cmake_minimum_required(VERSION 2.8)

project(voro-cpp)

# library
add_library(voro-cpp STATIC 
	config.hh
	common.hh
	common.cc
	cell.hh
	cell.cc
	unitcell.hh
	unitcell.cc
	pre_container.hh
	pre_container.cc
	container_prd.hh
	container_prd.cc
	container.hh
	container.cc
	v_compute.hh
	v_compute.cc
	worklist.hh
	rad_option.hh
	c_loops.hh
	c_loops.cc
	v_base.hh
	v_base.cc
	v_base_wl.cc
	wall.hh
	wall.inline.hh
	wall.cc
	voro++.hh
	)

add_executable(voro-console 
	cmd_line.cc
	)
	
target_link_libraries(voro-console
	voro-cpp
	)