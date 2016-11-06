//
//  Face.cpp
//  SUEX_SkinUp
//
//  Created by Tom Kaneko on 05/10/2016.
//  Copyright © 2016 Tom Kaneko. All rights reserved.
//

#include <cassert>

#include <SketchUpAPI/model/edge.h>

#include "Face.hpp"

namespace CW {

Face::Face(std::vector<Point3D> outer_loop, std::vector<std::vector<Point3D>> inner_loops):
	Face(create_face(outer_loop, inner_loops, m_create_result), true)
{
}


Face::Face(SUFaceRef face, bool release_on_destroy):
	m_face(face),
  m_release_on_destroy(release_on_destroy),
	DrawingElement(SUFaceToDrawingElement(face))
{
}
  
Face::~Face() {
	if (m_release_on_destroy) {
  	SUFaceRelease(&m_face);
  }
  //delete m_face;
}

  
SUFaceRef Face::create_face(std::vector<Point3D> outer_loop, std::vector<std::vector<Point3D>> inner_loops, SU_RESULT &create_result) {
	SUFaceRef face = SU_INVALID;
  size_t num_vertices = outer_loop.size();
  SUPoint3D* vertices[num_vertices];
  size_t j = 0;
  
  for (size_t i = 0; i < outer_loop.size(); ++i){
  	vertices[j] = outer_loop[i];
    ++j;
	}
  LoopInput loop_input{outer_loop};
  SULoopInputRef loop_input_ref = loop_input.ref();
  create_result = SUFaceCreate(&face, vertices[0], &loop_input_ref);
  // Add inner loops
  if (create_result == SU_ERROR_NONE) {
    for (size_t i = 0; i < inner_loops.size(); ++i){
      size_t num_il_vertices = inner_loops[i].size();
      SUPoint3D* vertices[num_il_vertices];
      LoopInput loop{inner_loops[i]};
      SULoopInputRef inner_loop_input = loop.ref();
      SU_RESULT res = SUFaceAddInnerLoop(face, vertices[0], &inner_loop_input);
      assert(res == SU_ERROR_NONE);
    }
  }
	return face;
}

/*
SUFaceRef Face::check_face(SUFaceRef face, SU_RESULT &create_result) {
  
}
*/
  
SUFaceRef Face::ref() {	return m_face; }
Face::operator SUFaceRef() {	return m_face;}
Face::operator SUFaceRef*() {	return &m_face;}

Face::operator bool() const {
	if (m_create_result == SU_ERROR_NONE) {
  	return true;
  }
  return false;
}

bool Face::operator!() const {
	return !bool(this);
}


double Face::area() const {
	double area;
	SU_RESULT res = SUFaceGetArea(m_face, &area);
  assert(res == SU_ERROR_NONE);
  return area;
}


Material Face::back_material() const {
	Material material{};
	SUFaceGetBackMaterial(m_face, material);
	return material;
}


Material Face::back_material(const Material material) {
  SUFaceSetBackMaterial(m_face, material);
  return material;
}

/*
* determine if a given Point3d is on the referenced Face. The return value is calculated from this list:
  PointUnknown (indicates an error),
  PointInside (point is on the face, not in a hole),
  PointOnVertex (point touches a vertex),
  PointOnEdge (point is on an edge),
  PointOutside (point outside the face or in a hole),
  PointNotOnPlane (point off the face's plane).
* @param SUPoint3D object.
* @return FacePointClass enum indicating the status of the point relative to the face.
*/
FacePointClass Face::classify_point(Point3D point) {
	// TODO
}


std::vector<Edge> Face::edges() {
	std::vector<Edge> total_edges = outer_loop().edges();
  std::vector<Loop> all_loops = loops();
  for (size_t i=0; i < all_loops.size(); i++) {
  	std::vector<Edge> loop_edges = all_loops[i].edges();
  	total_edges.reserve(total_edges.size() + loop_edges.size());
    total_edges.insert(total_edges.end(), loop_edges.begin(), loop_edges.end());
  }
  return total_edges;
}

UVHelper Face::get_UVHelper(bool front, bool back, TextureWriter tex_writer) {
	//SUUVHelperRef uv_helper = SU_INVALID;
  UVHelper uv_helper{};
  SUFaceGetUVHelper(m_face, front, back, tex_writer, uv_helper);
  return uv_helper;
}

/*
* Returns a vector representing the projection for either the front or back side of the face.
* @param bool true for frontside, false for back side.
*/
Vector3D Face::get_texture_projection(const bool frontside) const {
	//TODO
}


std::vector<Loop> Face::inner_loops() const {
	size_t num_loops = 0;
	SUFaceGetNumInnerLoops(m_face, &num_loops);
  SULoopRef inner_loops[num_loops];
  SUFaceGetInnerLoops(m_face, num_loops, &inner_loops[0], &num_loops);
  std::vector<Loop> loops(num_loops);
	for (size_t i=0; i < num_loops; i++) {
  	loops[i] = Loop(inner_loops[i]);
  }
  return loops;
}


std::vector<Loop> Face::loops() const {
	std::vector<Loop> all_loops;
  all_loops.push_back(outer_loop());
  std::vector<Loop> inner_ls = inner_loops();
  all_loops.reserve(all_loops.size() + inner_ls.size());
	all_loops.insert(all_loops.end(), inner_ls.begin(), inner_ls.end());
	return all_loops;
}

/*
// TODO another day.
PolygonMesh mesh();
*/


Vector3D Face::normal() const {
	Plane3D c_plane = plane();
  return c_plane.normal();
}


Loop Face::outer_loop() const {
  SULoopRef lp = SU_INVALID;;
  SUFaceGetOuterLoop(m_face, &lp);
  return Loop(lp);
}


Plane3D Face::plane() const {
	SUPlane3D plane = SU_INVALID;
	SUFaceGetPlane(m_face, &plane);
  return Plane3D(plane);
}

/*
* Positions a material on a face.
* The pt_array must contain 2, 4, 6 or 8 points. The points are used in pairs to tell where a point in the texture image is positioned on the Face. The first point in each pair is a 3D point in the model. It should be a point on the Face. The second point in each pair of points is a 2D point that gives the (u,v) coordinates of a point in the image to match up with the 3D point.
* @param Material object to position.
* @param vector of Point3d objects used to position the material.
* @param bool true to position the texture on the front of the Face or false to position it on the back of the Face.
*/
bool Face::position_material(const Material &material, std::vector<Point3D> pt_array, bool o_front) {
	// TODO
}


Face* Face::reverse() {
	SU_RESULT res = SUFaceReverse(m_face);
  if (res == SU_ERROR_NONE) {
  	return this;
  }
  else {
  	return nullptr;
  }
}

/*
* Sets the texture projection direction.
* @param SUVector3D object representing the direction of the projection. Or bool true to remove texture projection.
* @param bool true for front side, false for back side.
* @return true on success
*/
bool Face::set_texture_projection(const Vector3D vector, bool frontside) {
	// TODO
}
bool Face::set_texture_projection(bool remove, bool frontside) {
	// TODO
}


std::vector<Vertex> Face::vertices() const {
	size_t num_vertices = 0;
	SUFaceGetNumVertices(m_face, &num_vertices);
  SUVertexRef vertices[num_vertices];
  SUFaceGetVertices(m_face, num_vertices, &vertices[0], &num_vertices);
  std::vector<Vertex> verts(num_vertices);
  for (size_t i=0; i < num_vertices; i++) {
  	verts[i] = Vertex(vertices[i]);
  }
  return verts;
}
  
} /* namespace CW */