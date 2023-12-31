/********************************************************************************
 * Copyright 2009 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
 * Faculty of Engineering, University of Southern Denmark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************************/

#ifndef RW_GEOMETRY_TRIANGULATE_HPP
#define RW_GEOMETRY_TRIANGULATE_HPP

#if !defined(SWIG)
#include <rw/geometry/Polygon.hpp>
#include <rw/math/Vector2D.hpp>
#include <rw/math/Vector3D.hpp>

#include <vector>
#endif
namespace rw { namespace geometry {

    /**
     * @brief Class for triangulating a polygon
     */
    class Triangulate
    {
      public:
        /**
         * @brief triangulates a polygon or contuor described by a list of points
         * @param points [in] points must be a valid polygon/contour
         * @return a triangle mesh of triangles
         */
        // static PlainTriMeshN0D triangulatePolygon(const std::vector< rw::math::Vector3D<double>
        // >& points)

        /**
         * @brief Triangulates the polygon described by \b points
         *
         * The polygon need to be simple (no edges crossing and no holes). Both convex and concave
         * polygons are supported
         *
         * @param points [in] Points of the polygon. These needs to be in the right order, as no
         * sorting is done in the function.
         * @param result [out] Indices of the vertices defining the triangles. The corners of the
         * n'th triangle is the points with indices result[3*n], result[3*n+1] and result[3*n+2]
         */
        static bool processPoints (const std::vector< rw::math::Vector2D<> >& points,
                                   std::vector< int >& result);

        /**
         * @brief Triangulates the polygon described by \b points
         *
         * The polygon need to be simple (no edges crossing and no holes). Both convex and concave
         * polygons are supported
         *
         * @param contour [in] Points of the polygon. These needs to be in the right order, as no
         * sorting is done in the function.
         * @param result [out] Indices of the vertices defining the triangles. The corners of the
         * n'th triangle is the points with indices result[3*n], result[3*n+1] and result[3*n+2]
         * @param colinearCriteria [in] Criteria for when two edges are considered to be colinear
         * @param precision [in] Criteria for when two points are considered to be coinciding
         */
        static bool processPoints (const std::vector< rw::math::Vector3D< double > >& contour,
                                   std::vector< int >& result, double colinearCriteria = 1e-5,
                                   double precision = 1e-5);

        /**
         * @brief Triangules the polygon \b polygon
         *
         * The polygon need to be simple (no edges crossing and no holes). Both convex and concave
         * polygons are supported
         *
         * @param polygon [in] The polygon to triangulate
         * @param result [out] Indices of the vertices defining the triangles. The corners of the
         * n'th triangle is the points with indices result[3*n], result[3*n+1] and result[3*n+2]
         * @param colinearCriteria [in] Criteria for when two edges are considered to be colinear
         * @param precision [in] Criteria for when two points are considered to be coinciding
         */
        static bool processPolygon (Polygon<>::Ptr polygon, std::vector< int >& result,
                                    double colinearCriteria = 1e-5, double precision = 1e-5);

        /**
         * @brief Computes area of the polygon defined by \b contour
         */
        static double calcArea (const std::vector< rw::math::Vector2D<> >& contour);

        /**
         * @brief Checks if the point (Px,Py) is inside the triangle (Ax,Ay), (Bx,By), (Cx,Cy)
         */
        static bool insideTriangle2D (float Ax, float Ay, float Bx, float By, float Cx, float Cy,
                                      float Px, float Py);

      private:
        static bool snip (const std::vector< rw::math::Vector2D<> >& contour, int u, int v, int w,
                          int n, int* V);
    };
}}    // namespace rw::geometry

#endif    // end include guard
