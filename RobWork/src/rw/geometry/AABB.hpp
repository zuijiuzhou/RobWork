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

#ifndef RW_GEOMETRY_AABB_HPP_
#define RW_GEOMETRY_AABB_HPP_

#if !defined(SWIG)
#include <rw/geometry/BV.hpp>
#endif
#include <rw/common/Traits.hpp>
namespace rw { namespace geometry {

    /**
     * @brief Axis Aligned Bounding Box class
     */

#if !defined(SWIGJAVA)
    template< class T > class AABB : public BV< AABB< T > >
#else
    template< class T > class AABB
#endif
    {
      public:
        //! constructor
        AABB () : _position (0, 0, 0), _halfLng (0, 0, 0) {}

        /**
         * @brief Construct an AABB by defining the two oposit corners
         * @param corner1 [in] the first corner
         * @param corner2 [in] the second corner
         */
        AABB (const rw::math::Vector3D< T >& corner1, const rw::math::Vector3D< T >& corner2) :
            _position ((corner1 + corner2) / 2.0), _halfLng ((corner1 - corner2) / 2.0)
        {
            _halfLng[0] = abs (_halfLng[0]);
            _halfLng[1] = abs (_halfLng[1]);
            _halfLng[2] = abs (_halfLng[2]);
        }

        //! destructor
        virtual ~AABB () {}

        //! set half lengths
        inline void setHalfLengths (const rw::math::Vector3D< T >& pos) { _halfLng = pos; }

        //! get halflengths of this box
        inline const rw::math::Vector3D< T >& getHalfLengths () const { return _halfLng; }

        //! set position
        inline void setPosition (const rw::math::Vector3D< T >& pos) { _position = pos; }

        //! position of this AABB
        inline const rw::math::Vector3D< T >& getPosition () const { return _position; }

        //! @brief calculate the volume of this OBB
        inline T calcVolume () const
        {
            return _halfLng (0) * 2 * _halfLng (1) * 2 * _halfLng (2) * 2;
        }

        //! @brief calculates the total area of the box
        inline T calcArea () const
        {
            const T& h = _halfLng (0);
            const T& w = _halfLng (1);
            const T& l = _halfLng (2);
            return 2 * (h * 2 * w * 2) + 2 * (h * 2 * l * 2) + 2 * (w * 2 * l * 2);
        }

        /**
         * @brief returns the diagonal of the box
         * @return Vector3D<double>
         */
        rw::math::Vector3D< T > diagonal () { return _halfLng * 2; }

      private:
        rw::math::Vector3D< T > _position, _halfLng;
    };

}}    // namespace rw::geometry

#if !defined(SWIG)
extern template class rw::geometry::BV< rw::geometry::AABB< double > >;
extern template class rw::geometry::AABB< double >;
extern template class rw::geometry::BV< rw::geometry::AABB< float > >;
extern template class rw::geometry::AABB< float >;
#else
SWIG_DECLARE_TEMPLATE (BvAABB, rw::geometry::BV< rw::geometry::AABB< double > >);
#if SWIG_VERSION < 0x040000
SWIG_DECLARE_TEMPLATE (AABB_d, rw::geometry::AABB< double >);
ADD_DEFINITION (AABB_d, AABB, sdurw_geometry)
#else
SWIG_DECLARE_TEMPLATE (AABB, rw::geometry::AABB< double >);
#endif
#endif

#endif /* AABB_HPP_ */
