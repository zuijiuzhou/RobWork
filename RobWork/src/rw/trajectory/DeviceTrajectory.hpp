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

#ifndef RW_TRAJECTORY_DEVICETRAJECTORY_HPP
#define RW_TRAJECTORY_DEVICETRAJECTORY_HPP

#if !defined(SWIG)
#include <rw/trajectory/Path.hpp>
#include <rw/trajectory/Trajectory.hpp>
#endif
namespace rw { namespace models {
    class Device;
}}    // namespace rw::models

namespace rw { namespace trajectory {

    /**
     * @brief Implements a trajectory with blends between segments.
     * TODO: Briefly describe how
     *
     *
     */
    class DeviceTrajectory : public rw::trajectory::Trajectory< rw::math::Q >
    {
      public:
        /**
         * @brief Default constructor creating an empty trajectory
         */
        DeviceTrajectory (rw::core::Ptr< rw::models::Device > deviceIn,
                          const rw::kinematics::State& state);

        /**
         * @brief Destructor
         */
        virtual ~DeviceTrajectory ();

        //! @copydoc rw::trajectory::Trajectory::x(double) const
        rw::math::Q x (double t) const { return _trajectory->x (t); }

        //! @copydoc rw::trajectory::Trajectory::dx(double) const
        rw::math::Q dx (double t) const { return _trajectory->dx (t); }

        //! @copydoc rw::trajectory::Trajectory::ddx(double) const
        rw::math::Q ddx (double t) const { return _trajectory->ddx (t); }

        //! @copydoc rw::trajectory::Trajectory::duration()
        double duration () const { return _trajectory->duration (); }

        //! @copydoc rw::trajectory::Trajectory::startTime()
        double startTime () const { return _trajectory->startTime (); }

        //! @copydoc rw::trajectory::Trajectory::endTime()
        double endTime () const { return _trajectory->endTime (); }

        //! @copydoc rw::trajectory::Trajectory<T>::getIterator(double) const
        virtual typename rw::trajectory::TrajectoryIterator< T >::Ptr getIterator (double dt) const
        {
            return rw::core::ownedPtr (
                new BlendedTrajectoryIterator< T > (const_cast< BlendedTrajectory* > (this), dt));
        }

        // use default SE3 blend from last configuration to target
        void moveL (const rw::math::Transform3D<>& target, double blend = 0.0);

        // use default SE3 blend from last configuration to target
        void moveL (const rw::math::Rotation3D<>& target, double blend = 0.0);

        // use default SE3 blend from last configuration to target
        void moveL (const rw::math::Vector3D<>& target, double blend = 0.0);

        // use default blend from last configuration to target
        void moveJ (const rw::math::Q& target, double blend = 0.0);

      private:
        rw::core::Ptr< rw::models::Device > _dev;
        InterpolatorTrajectory< rw::math::Q >::Ptr _trajectory;
        double _time, _timeTotal;
        double _dt;
        rw::kinematics::State _state;
    };

}; }    // namespace rw::trajectory
}    // End namespaces

#endif
