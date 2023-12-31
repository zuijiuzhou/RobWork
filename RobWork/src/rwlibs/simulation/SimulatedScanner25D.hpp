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

#ifndef RWLIBS_SIMULATION_SIMULATEDSCANNER25D_HPP_
#define RWLIBS_SIMULATION_SIMULATEDSCANNER25D_HPP_

//! @file SimulatedScanner25D.hpp
#if !defined(SWIG)
#include <rwlibs/simulation/SimulatedSensor.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/sensor/Scanner25D.hpp>
#endif
namespace rwlibs { namespace simulation {
    class FrameGrabber25D;

    //! @addtogroup simulation
    // @{

    /**
     * @brief a simulated range scanner for 2.5D images, that is basically
     * pointclouds without color information.
     */
    class SimulatedScanner25D : public rwlibs::simulation::SimulatedSensor
    {
      public:
        //! @brief smart pointer type of this class
        typedef rw::core::Ptr< SimulatedScanner25D > Ptr;

      public:
        /**
         * @brief constructor
         * @param name [in] name of this simulated scanner
         * @param frame [in] the frame the scanner is attached to.
         * @param framegrabber [in] the framegrabber used for grabbing 2.5D images
         */
        SimulatedScanner25D (const std::string& name, rw::core::Ptr<rw::kinematics::Frame> frame,
                             rw::core::Ptr< rwlibs::simulation::FrameGrabber25D > framegrabber);

        /**
         * @brief constructor
         * @param name [in] name of this simulated scanner
         * @param desc [in] description of this scanner
         * @param frame [in] the frame the scanner is attached to.
         * @param framegrabber [in] the framegrabber used for grabbing 2.5D images
         */
        SimulatedScanner25D (const std::string& name, const std::string& desc,
                             rw::core::Ptr<rw::kinematics::Frame> frame,
                             rw::core::Ptr< rwlibs::simulation::FrameGrabber25D > framegrabber);

        /**
         * @brief destructor
         */
        virtual ~SimulatedScanner25D ();

        /**
         * @brief set the framerate in frames per sec.
         * @param rate [in] frames per sec
         */
        void setFrameRate (double rate);

        ///////////// below is inheritet functions form Scanner25D and Sensor

        //! @copydoc rw::sensor::Scanner25D::open
        void open ();

        //! @copydoc rw::sensor::Scanner25D::isOpen
        bool isOpen ();

        //! @copydoc rw::sensor::Scanner25D::close
        void close ();

        //! @copydoc rw::sensor::Scanner25D::acquire
        void acquire ();

        //! @copydoc rw::sensor::Scanner25D::isScanReady
        bool isScanReady ();

        //! @copydoc rw::sensor::Scanner25D::getRange
        std::pair< double, double > getRange ();

        //! @copydoc rw::sensor::Scanner25D::getFrameRate
        double getFrameRate ();

        //! @copydoc rw::geometry::PointCloud::getData
        const rw::geometry::PointCloud& getScan ();

        //! @copydoc SimulatedSensor::update
        void update (const Simulator::UpdateInfo& info, rw::kinematics::State& state);

        //! @copydoc SimulatedSensor::reset
        void reset (const rw::kinematics::State& state);

        /**
         * @brief get a handle to controlling an instance of the simulated sensor in a specific
         * simulator
         * @param instance [in] the simulator in which the handle is active
         */
        rw::core::Ptr<rw::sensor::Sensor> getSensorHandle (rw::core::Ptr<rwlibs::simulation::Simulator> instance);

        //! get instance of scanner
        rw::core::Ptr<rw::sensor::Scanner25D>
        getScanner25DSensor (rw::core::Ptr<rwlibs::simulation::Simulator> instance);

      private:
        rw::core::Ptr< FrameGrabber25D > _framegrabber;
        double _frameRate, _dtsum;
        bool _isAcquired, _isOpenned;
    };

    //! @}
}}    // namespace rwlibs::simulation

#endif /* SIMULATEDSCANNER25D_HPP_ */
