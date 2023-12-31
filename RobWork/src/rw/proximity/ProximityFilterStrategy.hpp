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

#ifndef RW_PROXIMITY_BROADPHASEDETECTOR_HPP
#define RW_PROXIMITY_BROADPHASEDETECTOR_HPP

#if !defined(SWIG)
#include <rw/proximity/ProximityCache.hpp>
#include <rw/proximity/ProximityFilter.hpp>
#include <rw/proximity/ProximitySetup.hpp>
#endif 

namespace rw { namespace geometry {
    class Geometry;
}}    // namespace rw::geometry
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace proximity {

    /**
     * @brief describe the interface of a broad phase proximity strategy or proximity culler.
     *
     * A broadphase strategy implement heuristics or rules for finding frame pairs that
     * are possibly overlapping and excluding framepairs that are definitely not overlapping.
     *
     * The interface supports early exiting by returning frame-pairs in an iterative manor. This
     * enables efficient collision filtering at the cost of ease of use. Before acquiring sets of
     * framepairs the update function need be called. Thereafter multiple calls to next
     * will return possibly colliding frame pairs.
     *
     * \code
     *
     * Filter f = bpstrategy->update(state)
     * while(f->hasNext()){
     *  FramePair fpair = f->next();
     * 	// do collision with narrowphase strategy
     *  ...
     * }
     * \endcode
     *
     */
    class ProximityFilterStrategy
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< ProximityFilterStrategy > Ptr;
        //! @brief smart pointer type to this const class
        typedef rw::core::Ptr< const ProximityFilterStrategy > CPtr;

        //! @brief Destructor
        virtual ~ProximityFilterStrategy (){};

        /**
         * @brief Reset
         * @param state [in] the state.
         */
        virtual void reset (const rw::kinematics::State& state) = 0;

        /**
         * @brief creates a FilterData object. This is used for caching relavant data between calls
         * to update
         *
         * @return
         */
        virtual rw::core::Ptr<rw::proximity::ProximityCache> createProximityCache () = 0;

        /**
         * @brief Do an update
         * @param state [in] the state.
         * @return
         */
        virtual rw::core::Ptr<rw::proximity::ProximityFilter> update (const rw::kinematics::State& state) = 0;

        /**
         * @brief called once before acquirering all possibly colliding
         * frame pairs in the workcell
         * @param state [in] the state for which collision detection is performed.
         * @param data
         */
        virtual rw::core::Ptr<rw::proximity::ProximityFilter> update (const rw::kinematics::State& state,
                                             rw::core::Ptr<rw::proximity::ProximityCache> data) = 0;

        /**
         * @brief get the proximity setup that describe the include/exclude rules of this
         * BroadPhaseStrategy
         * @return a reference to the ProximitySetup
         */
        virtual rw::proximity::ProximitySetup& getProximitySetup () = 0;

        /**
         * @brief Adds geometry associated to frame
         * @param frame [in] Frame which has the geometry associated
         * @param geo [in] Geometry
         */
        virtual void addGeometry (rw::core::Ptr<rw::kinematics::Frame> frame,
                                  const rw::core::Ptr< rw::geometry::Geometry > geo) = 0;

        /**
         * @brief Removes the geometric model \b geo associated with
         * Frame \b frame from this strategy.
         *
         * @param frame [in] Frame which has the geometry associated
         * @param geo [in] Geometry
         */
        virtual void removeGeometry (rw::core::Ptr<rw::kinematics::Frame> frame,
                                     const rw::core::Ptr< rw::geometry::Geometry > geo) = 0;

        /**
         * @brief Removes the geometric model with name \b geoName and which is associated with
         * \b frame.
         *
         * @param frame [in] Frame which has the geometry associated
         * @param geoName [in] Name of geometry
         */
        virtual void removeGeometry (rw::core::Ptr<rw::kinematics::Frame> frame, const std::string& geoName) = 0;

        /**
         * @brief Adds a ProximitySetupRule
         * @param rule [in] the rule to add.
         */
        virtual void addRule (const rw::proximity::ProximitySetupRule& rule) = 0;

        /**
         * @brief Removes a ProximitySetupRule
         * If the rule cannot be found, then noting happens.
         * @param rule [in] the rule to remove.
         */
        virtual void removeRule (const rw::proximity::ProximitySetupRule& rule) = 0;
    };

}}    // namespace rw::proximity

#endif /* RW_PROXIMITY_BROADPHASEDETECTOR_HPP */
