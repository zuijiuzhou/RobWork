/********************************************************************************
 * Copyright 2013 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RW_GEOMETRY_TUBE_HPP_
#define RW_GEOMETRY_TUBE_HPP_

#if !defined(SWIG) 
#include <rw/geometry/Primitive.hpp>
#endif 

namespace rw { namespace geometry {
    //! @addtogroup geometry
    // @{

    /**
     * @brief a tube primitive. radius is in xy-plane and height is in z-axis
     */
    class Tube : public Primitive
    {
      public:
        /**
         * @brief Constructs tube primitive with the specified setup
         *
         * The tube is aligned with the height in the z-direction such that tube extends height/2
         * above and below the xy-plane.
         *
         * @param radius [in] inner radius of the tube.
         * @param thickness [in] thickness of the tube.
         * @param height [in] height of the cylinder.
         * @param levels [in] granularity of the mesh.
         */
        Tube (float radius, float thickness, float height, int levels = 16);

        /**
         * @brief Constructs tube primitive based on vector with parameters
         *
         * @param initQ [in] vector of (radius, height)
         * @param levels [in] granularity of the mesh
         */
        Tube (const rw::math::Q& initQ, int levels = 16);

        //! @brief destructor
        virtual ~Tube ();

        /**
         * @brief Get the inner radius of the tube.
         * @return the radius
         */
        float getInnerRadius () const;

        /**
         * @brief Get the thickness of the tube.
         * @return the thickness.
         */
        float getThickness () const;

        /**
         * @brief Get the height of the tube.
         * @return the height
         */
        float getHeight () const;

        // inherited from Primitive

        //! @copydoc Primitive::createMesh
        TriMesh::Ptr createMesh (int resolution) const;

        //! @copydoc Primitive::getParameters
        virtual rw::math::Q getParameters () const;

        //! @copydoc Primitive::setParameters
        virtual void setParameters (const rw::math::Q& q);

        //! @copydoc GeometryData::getType
        GeometryType getType () const { return TubePrim; }

      protected:
        /**
         * @brief Check if point lies inside geometry.
         * @param point [in] point to check.
         * @return true if inside geometry, false otherwise.
         */
        bool doIsInside (const rw::math::Vector3D<double>& point);

      private:
        float _radius;
        float _thickness;
        float _height;
    };
    //! @}

}}     // namespace rw::geometry
#endif /* RW_GEOMETRY_TUBE_HPP_ */
