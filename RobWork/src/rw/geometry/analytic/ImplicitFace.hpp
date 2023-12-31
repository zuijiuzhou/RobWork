/******************************************************************************
 * Copyright 2019 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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
 ******************************************************************************/

#ifndef RW_GEOMETRY_ANALYTIC_IMPLICITFACE_HPP_
#define RW_GEOMETRY_ANALYTIC_IMPLICITFACE_HPP_

/**
 * @file ImplicitFace.hpp
 *
 * \copydoc rw::geometry::ImplicitFace
 */

#if !defined(SWIG)
#include <rw/geometry/analytic/Face.hpp>
#include <rw/geometry/analytic/ImplicitSurface.hpp>
#include <rw/geometry/analytic/ParametricCurve.hpp>
#endif
namespace rw { namespace geometry {

    //! @addtogroup geometry
#if !defined(SWIG)
    //! @{
#endif
    /**
     * @brief Type of Face, where the surface is an ImplicitSurface and the
     * edges are of type ParametricCurve.
     */
    #if !defined(SWIGJAVA)
    class ImplicitFace : public Face
    #else 
    class ImplicitFace
    #endif 

    {
      public:
        //! @brief Smart pointer type to ImplicitFace
        typedef rw::core::Ptr< ImplicitFace > Ptr;

        //! @brief Smart pointer type to const ImplicitFace
        typedef rw::core::Ptr< const ImplicitFace > CPtr;

        //! @brief Constructor.
        ImplicitFace ();

        /**
         * @brief Construct face with surface and vertices given initially.
         *
         * Curves must be set afterwards.
         *
         * @param surface [in] the surface data.
         * @param vertices [in] vector of vertices.
         */
        ImplicitFace (rw::core::Ptr< const rw::geometry::ImplicitSurface > surface,
                      const std::vector< rw::math::Vector3D<double> >& vertices);

        //! @brief Destructor.
        virtual ~ImplicitFace ();

        /**
         * @brief Get the surface of the face.
         * @return a reference to the surface data.
         */
        virtual const rw::geometry::ImplicitSurface& surface () const;

        //! @copydoc Face::curveCount
        virtual std::size_t curveCount () const { return _curves.size (); }

        //! @copydoc Face::getCurve
        virtual const rw::geometry::ParametricCurve& getCurve (std::size_t i) const;

        //! @copydoc Face::vertices
        virtual const std::vector< rw::math::Vector3D<double> >& vertices () const { return _vertices; }

        //! @copydoc Face::transform(const rw::math::Vector3D<double>&)
        virtual void transform (const rw::math::Vector3D<double>& P);

        //! @copydoc Face::transform(const rw::math::Transform3D<>&)
        virtual void transform (const rw::math::Transform3D<>& T);

        /**
         * @brief Get the parametric curves.
         * @return vector with the curves.
         */
        const std::vector< rw::core::Ptr< const rw::geometry::ParametricCurve > >& getCurves () const
        {
            return _curves;
        }

        /**
         * @brief Set implicit surface.
         * @param surface [in] the surface.
         */
        void setSurface (rw::core::Ptr< const rw::geometry::ImplicitSurface > surface) { _surface = surface; }

        /**
         * @brief Set surface.
         * @param surface [in] the surface.
         */
        void setSurface (const rw::geometry::ImplicitSurface& surface);

        /**
         * @brief Set parametric curve (a curve has direction)
         * @param vertex [in] the start vertex.
         * @param curve [in] the curve.
         */
        void setCurve (std::size_t vertex, rw::core::Ptr< const rw::geometry::ParametricCurve > curve);

        /**
         * @brief Set the parametric curves.
         * @param curves [in] vector of directed curves.
         */
        void setCurves (const std::vector< rw::core::Ptr< const rw::geometry::ParametricCurve > >& curves);

        /**
         * @brief Set vertex.
         * @param index [in] vertex index to set.
         * @param vertex [in] the vertex point.
         */
        void setVertex (std::size_t index, const rw::math::Vector3D<double>& vertex);

        /**
         * @brief Set the vertices.
         * @param vertices [in] vector of vertices.
         */
        void setVertices (const std::vector< rw::math::Vector3D<double> >& vertices);

      private:
        rw::core::Ptr< const rw::geometry::ImplicitSurface > _surface;
        std::vector< rw::core::Ptr< const rw::geometry::ParametricCurve > > _curves;
        std::vector< rw::math::Vector3D<double> > _vertices;
    };
#if !defined(SWIG)
    //! @}
#endif
}}    // namespace rw::geometry

#endif /* RW_GEOMETRY_ANALYTIC_IMPLICITFACE_HPP_ */
