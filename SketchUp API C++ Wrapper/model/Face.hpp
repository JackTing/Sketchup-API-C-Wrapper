//
//  Face.hpp
//  SUEX_SkinUp
//
//  Created by Tom Kaneko on 05/10/2016.
//  Copyright © 2016 Tom Kaneko. All rights reserved.
//

#ifndef Face_hpp
#define Face_hpp

#include <stdio.h>
#include <vector>

#include "Geometry.hpp"
#include "DrawingElement.hpp"
#include "Material.hpp"
#include "Vertex.hpp"
#include "Edge.hpp"
#include "Loop.hpp"
#include "LoopInput.hpp"
#include "TextureWriter.hpp"
#include "UVHelper.hpp"

#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/loop.h>

namespace CW {

enum FacePointClass {
	PointUnknown, // (indicates an error),
	PointInside, // (point is on the face, not in a hole),
	PointOnVertex, // (point touches a vertex),
	PointOnEdge, // (point is on an edge),
	PointOutside, // (point outside the face or in a hole),
	PointNotOnPlane // (point off the face's plane).
};

class Face :public DrawingElement {
	private:
  SUFaceRef m_face;
  bool m_release_on_destroy; // Indicates whether the SUFaceRef should be released on destruction of the object.
	SU_RESULT m_create_result = SU_ERROR_NONE;
  
  static SUFaceRef create_face(std::vector<Point3D> outer_loop, std::vector<std::vector<Point3D>> inner_loops, SU_RESULT &create_result);
  //static SUFaceRef check_face(SUFaceRef face, SU_RESULT &create_result);
  
  public:
  
  /*
  * Face constructor, which takes an array of points representing the outer loop, and an array of arrays of points representing the inner loops of the face.  A new SUFaceRef object will be created within this class and handled internally.
  */
  Face(std::vector<Point3D> outer_loop, std::vector<std::vector<Point3D>> inner_loops = {{}});
  
  /*
  * Face constructor that essentially wraps around an already created SUFaceRef object.
  * @param SUFaceRef* pointer to the face.
  * @param bool true if the face should be released when this class object is destroyed.  False, if the destruction of the face object is handled elsewhere (use with caution).
  */
  Face(SUFaceRef face, bool release_on_destroy = false);

  ~Face();
  
  /*
  * Returns the C-style face_ref object
  */
  SUFaceRef ref();
  
  /*
  * The class object can be converted to a SUFaceRef without loss of data.
  */
  operator SUFaceRef();
  operator SUFaceRef*();
  
  /*
  * Returns whether the class is a valid SU object.
  */
  operator bool() const;
  
  /**
  * NOT operator.  Checks if the object is valid.
  * @return true if the curve is invalid
  */
	bool operator!() const;
  
	/*
  * Retrieves the area of a face in SU units.
  * @return double area in square inches (SU units).
  */
  double area() const;
	
  /*
  * Retrieves the material assigned to the back side of the face.
  * @return Material object of the back side of the face
  */
  Material back_material() const;
  
  /*
  * Sets the material assigned to the back side of the face.
  * @param Material object or the name of a valid material.
  * @return Material object of the back side of the face
  */
  Material back_material(const Material material);
  
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
  FacePointClass classify_point(Point3D point);
  
  /*
  * Get an array of edges that bound the face, including the edges of inner loops.
  * @return std::vector of Edge objects that bound the Face.
  */
  std::vector<Edge> edges();
  
  /*
  * Retrieves a UVHelper object for use in texture manipulation on a face.
  * @param front true if you want the texture coordinates for the front face, false if not. Defaults to true.
  * @param back True if you want the texture coordinates for the back face, false if not. Defaults to true.
  * @param TextureWriter object.
  * @return a UVHelper object.
  */
  UVHelper get_UVHelper(bool front = true, bool back = true, TextureWriter tex_writer = TextureWriter());
  
  /*
  * Returns a vector representing the projection for either the front or back side of the face.
  * @param bool true for frontside, false for back side.
  */
  Vector3D get_texture_projection(const bool frontside) const;
  
  /*
  * Gets an array of all of the inner loops that bound the face.
  */
  std::vector<Loop> inner_loops() const;
  
  /*
  * Gets an array of all of the loops that bound the face.  The first Loop is the outer loop.
  */
  std::vector<Loop> loops() const;
  
  /*
  // TODO
  PolygonMesh mesh();
  */
  
  /*
  * Retrieve the 3D vector normal to the face in the front direction.
  */
  Vector3D normal() const;
  
  /*
  * Retrieves the outer loop that bounds the face.
  */
	Loop outer_loop() const;
  
  /*
  * Retreives the plane of this face.
  */
  Plane3D plane() const;
  
  /*
  * Positions a material on a face.
	* The pt_array must contain 2, 4, 6 or 8 points. The points are used in pairs to tell where a point in the texture image is positioned on the Face. The first point in each pair is a 3D point in the model. It should be a point on the Face. The second point in each pair of points is a 2D point that gives the (u,v) coordinates of a point in the image to match up with the 3D point.
  * @param Material object to position.
  * @param vector of Point3d objects used to position the material.
  * @param bool true to position the texture on the front of the Face or false to position it on the back of the Face.
  */
  bool position_material(const Material &material, std::vector<Point3D> pt_array, bool o_front);

	/*
  * Reverses the face's orientation, meaning the front becomes the back.
  * @return the reversed Face object if successful, nullptr if unsuccessful
  */
	Face* reverse();
  
  /*
  * Sets the texture projection direction.
  * @param SUVector3D object representing the direction of the projection. Or bool true to remove texture projection.
  * @param bool true for front side, false for back side.
  * @return true on success
  */
  bool set_texture_projection(const Vector3D vector, bool frontside);
  bool set_texture_projection(bool remove, bool frontside);
  
	/*
  * Gets an array of all of the vertices that bound the face.
  * @return std::vector array of Vertex objects.
  */
  std::vector<Vertex> vertices() const;
	
  /**
  * Returns the result of the SUFaceCreate operation.
  */
  SU_RESULT get_result() const;
  
};

} /* namespace CW */
#endif /* Face_hpp */
