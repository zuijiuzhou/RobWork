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

#ifndef RW_GEOMETRY_LINE_HPP_
#define RW_GEOMETRY_LINE_HPP_

#if !defined(SWIG)
#include <rw/geometry/Primitive.hpp>

#include <rw/math/Metric.hpp>

#include <iosfwd>
#endif

namespace rw { namespace geometry {

    //! @addtogroup geometry
    // @{
    /**
     * @brief A line in 3D, described by a two points.
     */
    class Line : public Primitive
    {
      public:
        //! @brief Smart pointer to Line.
        typedef rw::core::Ptr< Line > Ptr;

        //! @brief Type of internal values.
        typedef double value_type;

        /**
         * @brief Constructor.
         *
         * Default constructor returns line segment from {0, 0, 0} to {0, 0, 1}.
         */
        Line ();

        /**
         * @brief Constructor.
         * @param params [in] must be 6 long and contain 2 points lying on the line
         */
        Line (const rw::math::Q& params);

        /**
         * @brief Constructor.
         *
         * @param p1 [in] point 1.
         * @param p2 [in] point 2.
         */
        Line (const rw::math::Vector3D<double>& p1, const rw::math::Vector3D<double>& p2);

        //! @brief destructor
        virtual ~Line ();

        //! @brief Get point 1.
        inline rw::math::Vector3D<double>& p1 () { return _p1; }

        //! @brief Get point 1.
        inline const rw::math::Vector3D<double>& p1 () const { return _p1; }

        //! @brief Get point 2.
        inline rw::math::Vector3D<double>& p2 () { return _p2; }

        //! @brief Get point 2.
        inline const rw::math::Vector3D<double>& p2 () const { return _p2; }

        //! @brief Get a direction vector u = normalize(p2 - p1).
        inline rw::math::Vector3D<double> dir () const { return normalize (_p2 - _p1); }

        /**
         * @brief Calculates the shortest distance from a point to the line.
         *
         * For the purposes of this calculation, the line is treated as infinitely extending
         * geometric entity, without begining nor end.
         */
        double distance (const rw::math::Vector3D<double>& point) const;

        /**
         * @brief Calculates the shortest distance to another line.
         *
         * For the purposes of this calculation, the lines are treated as infinitely extending
         * geometric entity, without begining nor end.
         */
        double distance (const Line& line) const;

        /**
         * @brief Finds a point on the line closest to specified point.
         *
         * For the purposes of this calculation, the line is treated as infinitely extending
         * geometric entity, without begining nor end.
         */
        rw::math::Vector3D<double> closestPoint (const rw::math::Vector3D<double>& point) const;

        /**
         * @brief Fit this line to a set of points
         *
         * Uses centroid calculation and SVD analysis to determine the parameters of the line.
         * p1 is the point on the line closest to origin {0, 0, 0},
         * and p2 is chosen so as (p2 - p1) is an unit vector.
         * Error is the sum of point distances to the line squared.
         *
         * @param data [in] a set of points
         *
         * @return sum of the squares of point distances to the line
         */
        double refit (const std::vector< rw::math::Vector3D<double> >& data);

#if !defined(SWIGJAVA)
        /**
         * @copybrief refit(const std::vector<rw::math::Vector3D<double> >&)
         *
         * This version of refit makes it possible to fit only a subset of the points in a vector.
         *
         * @param begin [in] iterator to first element.
         * @param end [in] iterator to last element.
         * @return sum of the squares of point distances to the line.
         * @see refit(const std::vector<rw::math::Vector3D<double> >&)
         */

         #endif 
        double refit (const std::vector< rw::math::Vector3D<double> >::const_iterator begin,
                      const std::vector< rw::math::Vector3D<double> >::const_iterator end);

        // inherited from Primitive
        //! @copydoc Primitive::createMesh
        TriMesh::Ptr createMesh (int resolution) const { return NULL; }

        //! @copydoc Primitive::getParameters
        virtual rw::math::Q getParameters () const
        {
            return rw::math::Q (6, _p1[0], _p1[1], _p1[2], _p2[0], _p2[1], _p2[2]);
        }

        //! @copydoc Primitive::setParameters
        virtual void setParameters (const rw::math::Q& q);

        //! @copydoc Primitive::getType
        GeometryType getType () const { return LinePrim; }

        //! @brief Create set of lines making a grid.
        static std::vector< Line >
        makeGrid (int dim_x, int dim_y, double size_x = 1.0, double size_y = 1.0,
                  const rw::math::Vector3D<double>& xdir = rw::math::Vector3D<double>::x (),
                  const rw::math::Vector3D<double>& ydir = rw::math::Vector3D<double>::y ());

        /**
         * @brief create a metric that can be used to compare the difference between
         * two lines. The distance between two lines is computed as follows:
         *
         * val = 0.5*angle(l1.dir, l2.dir)*angToDistWeight + 0.5*l1.distance(l2)
         * @return distance metric
         */
        static rw::math::Metric< Line >::Ptr makeMetric (double angToDistWeight = 1.0);
#if !defined(SWIG)
        /**
           @brief Streaming operator.
         */
        friend std::ostream& operator<< (std::ostream& out, const Line& line)
        {
            return out << "Line("
                       << "p1: " << line._p1 << ", p2: " << line._p2 << ")";
        };
#else
        TOSTRING (rw::geometry::Line);
#endif

      private:
        rw::math::Vector3D<double> _p1, _p2;
    };
}}    // namespace rw::geometry
#if defined(SWIG)
SWIG_DECLARE_TEMPLATE (MetricLine, rw::math::Metric< rw::geometry::Line >);
#endif

namespace rw { namespace geometry {
    /**
     * @brief A metric for calculating line-to-line distance.
     */
    class LineMetric : public rw::math::Metric< rw::geometry::Line >
    {
      public:
        /**
         * @brief Construct new metric.
         * @param angToDistWeight [in] (optional) weighting of the angle to distance. Default is 1.
         */
        LineMetric (double angToDistWeight = 1.0) : _angToDistWeight (angToDistWeight) {}

      protected:
        //! @brief Calculates distance from the line to Z axis
        double doDistance (const Line& l) const
        {
            return doDistance (Line (rw::math::Vector3D<double> (), rw::math::Vector3D<double>::z ()), l);
        }

        /**
         * @brief Calculates distance between two lines
         *
         * The distance is calculated according to formula:
         *
         * val = 0.5*angle(l1.dir, l2.dir)*angToDistWeight + 0.5*distance(l1, l2)
         */
        double doDistance (const Line& a, const Line& b) const
        {
            // if the direction faces the other direction, the real angle is 180 deg - calculated
            // angle
            double ang = rw::math::angle (a.dir (), b.dir ());
            ang        = ((rw::math::Pi - ang) < ang) ? (rw::math::Pi - ang) : ang;
            return 0.5 * ang * _angToDistWeight + 0.5 * a.distance (b);
        }

        //! @copydoc rw::math::Metric::doSize
        int doSize () const { return -1; }

        //! @brief The weighting of the angle to the distance.
        double _angToDistWeight;
    };
    // @}
}}    // namespace rw::geometry

#endif /* LINE_HPP_ */
