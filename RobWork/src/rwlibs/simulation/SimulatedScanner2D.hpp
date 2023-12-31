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

#ifndef RWLIBS_SIMULATION_SIMULATEDSCANNER2D_HPP
#define RWLIBS_SIMULATION_SIMULATEDSCANNER2D_HPP

//! @file SimulatedScanner2D.hpp
#if !defined(SWIG)
#include <rwlibs/simulation/SimulatedSensor.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/sensor/Scanner2D.hpp>
#include <rw/sensor/Scanner2DModel.hpp>
#endif
namespace rwlibs { namespace simulation {
    // forward declaration
    class Simulator;
    class FrameGrabber25D;
    //! @addtogroup simulation
    // @{

    /**
     * @brief Simulated scanner in 2D.
     */
    class SimulatedScanner2D : public SimulatedSensor
    {
      public:
        //! @brief smart pointer of this class
        typedef rw::core::Ptr< SimulatedScanner2D > Ptr;

        /**
         * @brief constructor
         * @param name [in] name of this simulated scanner
         * @param frame [in] the sensor frame.
         * @param framegrabber [in] the framegrabber used for grabbing 2.5D images
         */
        SimulatedScanner2D (const std::string& name, rw::core::Ptr<rw::kinematics::Frame> frame,
                            rw::core::Ptr< rwlibs::simulation::FrameGrabber25D > framegrabber);

        /**
         * @brief constructor
         * @param name [in] name of this simulated scanner
         * @param desc [in] description of this scanner
         * @param frame [in] the sensor frame.
         * @param framegrabber [in] the framegrabber used for grabbing 2.5D images
         */
        SimulatedScanner2D (const std::string& name, const std::string& desc,
                            rw::core::Ptr<rw::kinematics::Frame> frame,
                            rw::core::Ptr< rwlibs::simulation::FrameGrabber25D > framegrabber);

        /**
         * @brief destructor
         */
        virtual ~SimulatedScanner2D ();

        /**
         * @brief set the framerate in frames per sec.
         * @param rate [in] frames per sec
         */
        void setFrameRate (double rate);

        ///////////// below is inheritet functions form Scanner25D and Sensor

        //! @copydoc rw::sensor::Scanner2D::open
        void open ();

        //! @copydoc rw::sensor::Scanner2D::isOpen
        bool isOpen ();

        //! @copydoc rw::sensor::Scanner2D::close
        void close ();

        //! @copydoc rw::sensor::Scanner2D::acquire
        void acquire ();

        //! @copydoc rw::sensor::Scanner2D::isScanReady
        bool isScanReady ();

        //! @copydoc rw::sensor::Scanner2D::getRange
        std::pair< double, double > getRange ();

        //! @copydoc rw::sensor::Scanner2D::getFrameRate
        double getFrameRate ();

        //! @copydoc rwlibs::simulation::SimulatedKinect::getScan
        const rw::geometry::PointCloud& getScan () const;

        //! @copydoc SimulatedSensor::update
        void update (const rwlibs::simulation::Simulator::UpdateInfo& info, rw::kinematics::State& state);

        //! @copydoc SimulatedSensor::reset
        void reset (const rw::kinematics::State& state);

        /**
         * @brief returns a handle to what represents a statefull interface.
         * The handle will be locked to the simulator
         * @return
         */
        rw::core::Ptr<rw::sensor::Scanner2D> getScanner2DSensor (rwlibs::simulation::Simulator* instance);
#if !defined(SWIGJAVA)
        /**
         * @brief Get a model of the sensor.
         * @return the model.
         */
        rw::core::Ptr<rw::sensor::Scanner2DModel> getSensorModel ();
#endif
        //! @copydoc rw::sensor::Scanner2DModel::getAngularRange
        virtual double getAngularRange ();

        virtual size_t getMeasurementCount () const;

      private:
        rw::core::Ptr< FrameGrabber25D > _framegrabber;
        double _frameRate, _dtsum;
        bool _isAcquired, _isOpenned;
        rw::sensor::Scanner2DModel::Ptr _smodel;
    };

    //! @}
}}    // namespace rwlibs::simulation

#endif /* RWLIBS_SIMULATION_SIMULATEDSCANNER2D_HPP */
