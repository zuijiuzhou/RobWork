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

#ifndef PROXIMITYMODEL_HPP_
#define PROXIMITYMODEL_HPP_

#if !defined(SWIG)
#include <rw/core/Ptr.hpp>

#include <string>
#include <vector>
#endif 

namespace rw { namespace geometry {
    class Geometry;
}}    // namespace rw::geometry
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace proximity {

    class ProximityStrategy;

    /**
     * @brief Class for managing the collision geometries associated to a frame
     **/
    class ProximityModel
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< ProximityModel > Ptr;

        /**
         * @brief Constructor
         *
         * @param pOwner the ProximityStrategy owning this ProximityModel
         **/
        ProximityModel (ProximityStrategy* pOwner) : owner (pOwner), _frame (NULL) {}

        virtual ~ProximityModel ();

        /**
         * @brief return vector of names for the geometries added to this ProximityModel
         *
         **/
        std::vector< std::string > getGeometryIDs ();

        /**
         * @brief get the associated Geometries
         * @return a list of Geomety pointers beloninh to the model
         */
        std::vector< rw::core::Ptr< rw::geometry::Geometry > > getGeometries ();

        /**
         * @brief adds geometry
         * @param geom the geometry to add
         **/
        bool addGeometry (const rw::geometry::Geometry& geom);

        /**
         * @brief adds geometry using pointer
         * @param geom [in] the geometry to add
         * @param forceCopy [in]
         **/
        bool addGeometry (rw::core::Ptr< rw::geometry::Geometry > geom, bool forceCopy = false);

        /**
         * @brief removes a geometry from the ProximityModel
         *
         * @param geoid name of geometry to remove
         * @return bool
         **/
        bool removeGeometry (const std::string& geoid);

        /**
         * @brief return pointer to the associated frame
         *
         **/
        rw::kinematics::Frame* getFrame () { return _frame.get(); };

        /**
         * @brief sets the associated frame
         *
         * @param frame frame to set
         **/

        //void setFrame (const rw::core::Ptr<rw::kinematics::Frame> frame) { _frame = const_cast<rw::core::Ptr<rw::kinematics::Frame>> (frame); }
        void setFrame (rw::core::Ptr<rw::kinematics::Frame> frame) {_frame = frame;}

        ProximityStrategy* owner;

      private:
        rw::core::Ptr<rw::kinematics::Frame> _frame;
    };
}}    // namespace rw::proximity

#endif /* PROXIMITYMODEL_HPP_ */
