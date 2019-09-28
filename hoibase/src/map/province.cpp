// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/map/province.hpp"

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesh_vertex_base_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/lloyd_optimize_mesh_2.h>

namespace openhoi {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_mesh_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;

// Province constructor
Province::Province(std::string id,
                   std::vector<std::vector<Ogre::Vector2>> coordinates,
                   Ogre::Vector2 center) {
  this->id = id;
  this->coordinates = coordinates;
  this->center = center;
}

// Gets the resource's byte data
std::string const& Province::getID() const { return this->id; }

// Gets the province coordinates
std::vector<std::vector<Ogre::Vector2>> const& Province::getCoordinates()
    const {
  return coordinates;
}

// Gets the vertices of the triangulated province
std::vector<Ogre::Real> const Province::getTriangulatedVertices() const {
  // Create province vertex handles and insert contraints
  CDT cdt;
  std::vector<Vertex_handle> handles;
  size_t handleTotal = 0;
  for (const auto& coords : this->coordinates) {
    // Create vertex handles
    const size_t len = coords.size();
    for (size_t i = 0; i < len; i++) {
      const auto point = coords[i];
      handles.push_back(cdt.insert(Point(point.x, point.y)));
    }
    // Insert constraints
    for (size_t i = handleTotal; i < handleTotal + len; i++) {
      if (i < handleTotal + len - 1) {
        cdt.insert_constraint(handles[i], handles[i + 1]);
      } else {
        cdt.insert_constraint(handles[i], handles[handleTotal]);
      }
    }
    handleTotal += len;
  }

  // Mesh the domain
  CGAL::refine_Delaunay_mesh_2(cdt, Criteria());

  // Optimize mesh
  CGAL::lloyd_optimize_mesh_2(cdt, CGAL::parameters::max_iteration_number = 10);

  // Build vertices vector from triangles
  std::vector<Ogre::Real> vertices;
  for (CDT::Finite_faces_iterator it = cdt.finite_faces_begin();
       it != cdt.finite_faces_end(); ++it) {
    // Check if the triangle is in domain which means that it is inside
    // a province and is not a triangle connecting some province panhandles
    // etc.
    if (it->is_in_domain() && it->is_valid()) {
      // Get the triangle
      CDT::Triangle tri = cdt.triangle(it);
      for (int i = 0; i < 3; i++) {
        auto vertex = tri.vertex(i);
        vertices.push_back((Ogre::Real)(vertex.x() / 180));
        vertices.push_back((Ogre::Real)(vertex.y() / 85));
        vertices.push_back(0);
      }
    }
  }

  // Return vertices vector
  return vertices;
}

// Gets the province center point
Ogre::Vector2 const& Province::getCenter() const { return center; }

}  // namespace openhoi