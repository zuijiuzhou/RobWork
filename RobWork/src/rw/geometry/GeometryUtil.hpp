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

#ifndef RW_GEOMETRY_GEOMETRYUTIL_HPP_
#define RW_GEOMETRY_GEOMETRYUTIL_HPP_

#if !defined(SWIG)
#include <rw/core/Ptr.hpp>
#include <rw/math/InertiaMatrix.hpp>
#include <rw/math/LinearAlgebra.hpp>
#include <rw/math/Math.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/math/VectorND.hpp>

#include <vector>
#endif

namespace rw { namespace geometry {
    class TriMesh;
}}    // namespace rw::geometry
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace geometry {
    class Geometry;

    /** @addtogroup geometry */
    /*@{*/
    /**
     * @brief Utility functions for calculating properties of geometry
     *
     * The methods for calculation of volume, inertia, and the center of gravity, is as described in
     * [1].
     *
     * [1]: Fast and Accurate Computation of Polyhedral Mass Properties, Brian Mirtich. Journal of
     * Graphics Tools, Vol.1, pages 31-58, 1996
     */
    class GeometryUtil
    {
      public:
        /**
         * @brief Estimates the volume of a list of geometries.
         * @note If geometries are overlapping, the overlapping regions will count twice in the
         * volume.
         * @param geoms [in] the list of geometries.
         * @return the total volume of the geometries.
         */
        static double estimateVolume (const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms);

        /**
         * @brief Estimates the volume of a trimesh.
         * @param trimesh [in] the trimesh.
         * @return the total volume of the trimesh.
         */
        static double estimateVolume (const rw::geometry::TriMesh& trimesh);

        /**
         * @brief Estimates the inertia and center of gravity (COG) of a list of geometries.
         *
         * The inertia is described relative to the ref coordinate system translated to COG.
         * The \b reftrans parameter can however be used to transform the geometries.
         *
         * @param mass [in] the total mass of all geometries.
         * @param geoms [in] the list of geometries.
         * @param ref [in] the reference frame for the geometries (if NULL, it is assumed that
         * the geometries are defined relative to the same frame).
         * @param state [in] state used to retrieve the current location of geometries relative to
         * the reference frame (only used if ref is given).
         * @param reftrans [in] (optional) used to transform the geometry before calculation of the
         * inertia.
         * @return the center of gravity relative to the ref frame and the inertia around
         * the center of gravity (in the coordinate frame of the ref frame).
         */
        static std::pair< rw::math::Vector3D<double>, rw::math::InertiaMatrix<> > estimateInertiaCOG (
            double mass, const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms,
            const rw::core::Ptr<rw::kinematics::Frame> ref, const rw::kinematics::State& state,
            const rw::math::Transform3D<>& reftrans = rw::math::Transform3D<>::identity ());

        /**
         * @brief Estimates the inertia of a list of geometries.
         *
         * The inertia is described relative to the ref coordinate system
         * The \b reftrans parameter can however be used to transform the geometries.
         *
         * @param mass [in] the total mass of all geometries.
         * @param geoms [in] the list of geometries.
         * @param ref [in] the reference frame for the geometries (if NULL, it is assumed that
         * the geometries are defined relative to the same frame).
         * @param state [in] state used to retrieve the current location of geometries relative to
         * the reference frame (only used if ref is given).
         * @param reftrans [in] (optional) used to transform the geometry before calculation of the
         * inertia.
         * @return the inertia around relative to the ref frame.
         */
        static rw::math::InertiaMatrix<> estimateInertia (
            double mass, const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms,
            const rw::core::Ptr<rw::kinematics::Frame> ref, const rw::kinematics::State& state,
            const rw::math::Transform3D<>& reftrans = rw::math::Transform3D<>::identity ());

        /**
         * @brief Estimates the inertia of a list of geometries.
         *
         * The inertia is described relative to the geometry reference frame.
         * The \b reftrans parameter can however be used to transform the geometries.
         *
         * @note The geometries should be defined relative to the same frame - otherwise the
         * result will not make sense.
         *
         * @param mass [in] the total mass of all geometries.
         * @param geoms [in] the list of geometries.
         * @param reftrans [in] (optional) used to transform the geometry before calculation of the
         * inertia.
         * @return the inertia matrix relative to the reference frame.
         */
        static rw::math::InertiaMatrix<> estimateInertia (
            double mass, const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms,
            const rw::math::Transform3D<>& reftrans = rw::math::Transform3D<>::identity ());

        /**
         * @brief Find the principal axes and the principal inertia.
         * @param inertia [in] the inertia matrix to calculate principal values for.
         * @return a rotation matrix giving the principal axes, and a vector of principal inertia
         * values for these axes.
         */
        template< class T >
        static std::pair< rw::math::Rotation3D< T >, rw::math::Vector3D< T > >
        calculatePrincipalInertia (const rw::math::InertiaMatrix< T >& inertia)
        {
            const Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > inertiaEigen (inertia.e ());
            std::pair< typename rw::math::LinearAlgebra::EigenMatrix< T >::type,
                       typename rw::math::LinearAlgebra::EigenVector< T >::type >
                dec = rw::math::LinearAlgebra::eigenDecompositionSymmetric (inertiaEigen);
            const rw::math::Vector3D< T > principalInertia = rw::math::Vector3D< T > (dec.second);
            if (dec.first.determinant () < 0) {
                // reflect in z=0 plane
                dec.first (2, 0) = -dec.first (2, 0);
                dec.first (2, 1) = -dec.first (2, 1);
                dec.first (2, 2) = -dec.first (2, 2);
            }
            const rw::math::Rotation3D< T > rotation = rw::math::Rotation3D< T > (dec.first);
            return std::pair< rw::math::Rotation3D< T >, rw::math::Vector3D< T > > (
                rotation, principalInertia);
        }

        /**
         * @brief Estimates the center of gravity (COG) of a list of geometries.
         *
         * The COG will be found relative to the geometry frame.
         *
         * @note The geometries should be defined relative to the same frame - otherwise the
         * result will not make sense.
         *
         * @param geoms [in] the list of geometries.
         * @return the center of gravity for the geometries.
         */
        static rw::math::Vector3D<double>
        estimateCOG (const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms);

        /**
         * @brief Estimates the center of gravity (COG) of a list of geometries.
         *
         * The COG will be given relative to the given reference frame.
         *
         * @param geoms [in] the list of geometries.
         * @param ref [in] the reference frame.
         * @param state [in] the state which gives the position of the geometries relative to the
         * reference frame.
         * @return the center of gravity for the geometries.
         */
        static rw::math::Vector3D<double>
        estimateCOG (const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms,
                     const rw::core::Ptr<rw::kinematics::Frame> ref, const rw::kinematics::State& state);

        /**
         * @brief Estimates the center of gravity (COG) of a triangle mesh.
         * @param trimesh [in] the triangle mesh.
         * @param t3d [in] (optional) make a transformation of the trimesh.
         * @return the center of gravity of the mesh.
         */
        static rw::math::Vector3D<double>
        estimateCOG (const TriMesh& trimesh,
                     const rw::math::Transform3D<>& t3d = rw::math::Transform3D<>::identity ());

        /**
         * @brief calculates the max distance to any triangle in the geoms, from some point \b
         * center
         * @param geoms [in] the geometries containing the triangles
         * @param center [in] the point to calculate the distance from
         * @param ref [in] the reference frame.
         * @param state
         * @return the maximum distance to any triangle in the geometries
         */
        static double calcMaxDist (const std::vector< rw::core::Ptr< rw::geometry::Geometry > >& geoms,
                                   const rw::math::Vector3D<double> center, rw::core::Ptr<rw::kinematics::Frame> ref,
                                   const rw::kinematics::State& state);

        /**
         * @brief util function that locates all frames that is staticly connected to f
         * and that has geometry information.
         */
        static std::vector< rw::kinematics::Frame* >
        getAnchoredFrames (rw::kinematics::Frame& f, const rw::kinematics::State& state);

        /**
         * @brief util function that locates all frames in the sub tree of parent
         * that is staticly connected and that has geometry information.
         */
        static std::vector< rw::kinematics::Frame* >
        getAnchoredChildFrames (rw::core::Ptr<rw::kinematics::Frame> parent, const rw::kinematics::State& state);

        /**
         * @brief calculates volume of k-simplex
         * @warning What is meant by volume is not really volume, see "actualSimplexVolume" - if you
         want the volume.
         *                 Read the documentation (preferably code), to make sure you get what you
         want.
         *
         * Volume of k-dimensional simplex (triangle is 2-simplex) is calculated as:
         * \f$ V(S) = \frac{1}{k!} \sqrt{W W^T}\f$, where:
         *
         * \f$ W \f$ is a matrix consisting of rows
         *
         \f$w_i = [v_{i,1}-v_{0,1}\:v_{i,2}-v_{0, 2}\:\cdots\:v_{i,k+1}-v_{0,k+1}] \f$

         * of i-th vertex coordinates.
         * (taken from: http://www.math.niu.edu/~rusin/known-math/97/volumes.polyh) (dead link)
         */
        template< std::size_t N >
        static double simplexVolume (const std::vector< rw::math::VectorND< N > >& vertices)
        {
            RW_ASSERT (vertices.size () > 0);
            double volume = 0.0;

            // construct W matrix
            Eigen::Matrix< double, N - 1, N > W;
            for (int idx = 1; idx < static_cast< int > (vertices.size ()); ++idx) {
                // vector w_i
                Eigen::Matrix< double, 1, N > wi =
                    (vertices[idx].e () - vertices[0].e ()).transpose ();
                // std::cout << wi << std::endl;
                W.row (idx - 1) = wi;
            }
            // std::cout << W << std::endl;
            // std::cout << sqrt((W*W.transpose()).determinant()) << std::endl;

            // calculate volume
            volume = 1.0 / rw::math::Math::factorial (N - 1) *
                     sqrt ((W * W.transpose ()).determinant ());    // this sometimes gives NaN
            // even if matrix has a determinant...

            // now, for a bit of wishful thinking:
            if (rw::math::Math::isNaN (volume) /* std::isnan(volume) */) {
                volume = 0.0;
            }

            return volume;
        }

        /**
         * @brief Returns the extremum distances for the vertices of the TriMesh given the specified
         * transformation
         * @param trimesh [in] TriMesh to find extremum distances for
         * @param t3d [in] Transformation of the vertices
         * @return Pair containing the lower and upper extremum distances of the vertices.
         */
        static std::pair< rw::math::Vector3D<double>, rw::math::Vector3D<double> > getExtremumDistances (
            rw::core::Ptr< TriMesh > trimesh,
            const rw::math::Transform3D<>& t3d = rw::math::Transform3D<>::identity ());

        /**
         * @brief Returns the dimensions of \b geometry
         * @param geometry [in] Geometry to analyse
         * @return Dimensions in the x,y and z directions.
         */
        static rw::math::Vector3D<double> getDimensions (rw::core::Ptr< rw::geometry::Geometry > geometry);

        /**
         * @brief Returns the dimensions of \b trimesh
         * @param trimesh [in] TriMesh to analyse
         * @return Dimensions in the x,y and z directions.
         */
        static rw::math::Vector3D<double> getDimensions (rw::core::Ptr< TriMesh > trimesh);

        /**
         * @brief Calculates the volume of a N-simplex.
         * Volume of a N-simplex is calculated as:
         * \f$ V_{Simplex} = \frac{|W|}{N!} \f$, where:
         *
         * \f$ W \f$ is the N*N matrix \f$ [v_{1}-v_{0} , \:v_{i}-v_{0}\:\cdots\: , v_{N}-v_{0}] \f$
         * Where \f$ v_{i} \f$ is the i'th vertic given.
         * This function can be supplied with N and N+1 vertices.
         * If only N vertices are given, then the volume is calculated with the origo as the
         * remaining vertex. (See https://en.wikipedia.org/wiki/Simplex#Volume)
         */
        template< std::size_t N >
        static double actualSimplexVolume (const std::vector< rw::math::VectorND< N > >& vertices)
        {
            RW_ASSERT (vertices.size () != N + 1 || vertices.size () != N);
            double volume = 0.0;

            // construct W matrix
            Eigen::Matrix< double, N, N > W;
            if (vertices.size () == N + 1) {
                for (size_t idx = 0; idx < N; idx++) {
                    W.col (idx) = (vertices.at (idx + 1).e () - vertices.at (0).e ());
                }
            }
            else {
                for (size_t idx = 0; idx < N; idx++) {
                    W.col (idx) = vertices.at (idx).e ();
                }
            }

            // calculate volume
            volume =
                std::fabs (W.determinant ()) /
                rw::math::Math::factorial (static_cast< int > (N));    // this sometimes gives NaN
            // even if matrix has a determinant...

            // now, for a bit of wishful thinking:
            if (rw::math::Math::isNaN (volume)) {
                volume = 0.0;
            }
            return volume;
        }
    };
    //! @}
}}     // namespace rw::geometry
#endif /*RW_GEOMETRY_GEOMETRYUTIL_HPP_*/
