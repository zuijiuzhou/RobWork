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

#ifndef RW_KINEMATICS_FRAMEMAP_HPP
#define RW_KINEMATICS_FRAMEMAP_HPP

#if !defined(SWIG)
#include <rw/kinematics/Frame.hpp>
#include <rw/math/Transform3D.hpp>

#include <vector>
#endif

namespace rw { namespace kinematics {

    /**
     * @brief a specialized mapping implementation for frames. It uses the internal
     * structure of Frames to provide fast O(1) lookup for mappings from Frame to anything.
     *
     * @note A requirement is that all frames must be registered in the same StateStructure.
     */
    template< class T > class FrameMap
    {
      public:
        /**
         * @brief creates a framemap
         * @param s [in] the default value of new instances of T
         */
        FrameMap (int s = 20) : _initialSize (s), _defaultVal (false, T ()), _map (s, _defaultVal)
        {}

        /**
         * @brief creates a framemap with an initial size of s
         * @param s [in] nr of elements of the types T with default value "defaultVal"
         * @param defaultVal [in] the default value of new instances of T
         */

#if defined(SWIGLUA)
        FrameMap (const T& defaultVal, int s) :
#else
        FrameMap (const T& defaultVal, int s = 20) :
#endif
            _initialSize (s), _defaultVal (false, defaultVal), _map (s, _defaultVal)
        {}

        /**
         * @brief inserts a value into the frame map
         * @param frame [in] the frame for which the value is to be associated
         * @param value [in] the value that is to be associated to the frame
         */
        void insert (const rw::kinematics::Frame& frame, const T& value)
        {
            operator[] (frame) = value;
        }

        /**
           @brief True iff a value for \b frame has been inserted in the map (or
           accessed using non-const operator[]).
        */
        bool has (const rw::kinematics::Frame& frame)
        {
            const int idx = frame.getID ();
            resizeIfNeeded (idx);
            return _map[idx].first;
        }
#if !defined(SWIG)
        /**
           @brief return a reference to the value that is associated with the
           frame \b frame.

           If no value has been inserted for \b frame, then the default value of
           \b T is returned. Use has() to see if a value has been stored for \b
           frame.

           @param frame [in] the frame for which to find its associated values.
           @return reference to the value associated to frame.
        */
        const T& operator[] (const rw::kinematics::Frame& frame) const
        {
            const int idx = frame.getID ();
            resizeIfNeeded (idx);
            return _map[idx].second;
        }

        /**
           @brief return a reference to the value that is associated with the
           frame \b frame

           If no value has been inserted for \b frame, then the default value of
           \b T is inserted in the map and returned.

           @param frame [in] the frame for which to find its associated values.
           @return reference to the value associated to frame.
        */
        T& operator[] (const rw::kinematics::Frame& frame)
        {
            const int idx = frame.getID ();
            resizeIfNeeded (idx);
            OkVal& val = _map[idx];
            val.first  = true;
            return val.second;
        }
#else
        MAPOPERATOR (T, const rw::kinematics::Frame&);
#endif
        /**
         * @brief Erase an element from the map
         */
        void erase (const rw::kinematics::Frame& frame)
        {
            const int idx   = frame.getID ();
            _map[idx].first = false;
        }

        /**
           @brief Clear the frame map.
        */
        void clear ()
        {
            _map.clear ();
            _map.resize (_initialSize, _defaultVal);
        }

      private:
        void resizeIfNeeded (int idx) const
        {
            const int n = (int) _map.size ();
            if (idx >= n) {
                const int newSize = idx >= 2 * n ? idx + 1 : 2 * n;
                _map.resize (newSize, _defaultVal);
            }
        }

      private:
        typedef std::pair< bool, T > OkVal;

        int _initialSize;
        OkVal _defaultVal;
        mutable std::vector< OkVal > _map;
    };
#if !defined(SWIG)
    extern template class rw::kinematics::FrameMap< rw::math::Transform3D< double > >;
#else
    #if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (FrameMap_d, rw::kinematics::FrameMap< double >);
    ADD_DEFINITION (FrameMap_d, FrameMap,sdurw_geometry)
#else
    SWIG_DECLARE_TEMPLATE (FrameMap, rw::kinematics::FrameMap< double >);
#endif
#endif
}}    // namespace rw::kinematics

#endif /*RW_KINEMATICS_FRAMEMAP_HPP*/
