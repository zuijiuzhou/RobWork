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

#ifndef RWLIBS_SIMULATION_GLFRAMEGRAPPER25D_HPP
#define RWLIBS_SIMULATION_GLFRAMEGRAPPER25D_HPP

//! @file GLFrameGrabber25D.hpp
#if !defined(SWIG)
#include <rwlibs/simulation/FrameGrabber25D.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/graphics/SceneViewer.hpp>
#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace kinematics {
    class Frame;
    class State;
}}    // namespace rw::kinematics

namespace rwlibs { namespace simulation {
    //! @addtogroup simulation
    // @{

    /**
     * @brief An implementation of the FrameGrabber interface. The GLFrameGrabber25D
     * grabs images from a OpenGL scene using a simple pinhole camera model.
     *
     * a framethe opengl rendering to
     * take pictures of the scene.
     *
     * The most basic parameter of a camera is its Field of view. This
     * can be used as an initial camera model. Field of view can be
     * calculated from the focal length and the size of the CCD
     * typically (1/2, 1/3, 1/4) inch.
     * If a more realistic camera model is
     * required the perspective transform of a specific camera can be added
     */
    class GLFrameGrabber25D : public FrameGrabber25D
    {
      public:
        //! @brief Smart pointer type for GLFrameGrabber25D.
        typedef rw::core::Ptr< GLFrameGrabber25D > Ptr;

        /**
         * @brief constructor
         * @param width [in] width of image
         * @param height [in] height of image
         * @param fov [in] the vertical field of view angle in degree
         * @param mindepth [in] the minimum depth of camera.
         * @param maxdepth [in] the maximum depth of camera.
         */
        GLFrameGrabber25D (int width, int height, double fov, double mindepth = 0.1,
                           double maxdepth = 10.0);

        /**
         * @brief destructor
         */
        virtual ~GLFrameGrabber25D ();

        /**
         * @brief initialize the grabber with a scene viewer. This registers the grabber
         * as a camera in the scene and enables rendering.
         * @param drawer [in] the scene viewer
         * @return true if initialization succeeded, false otherwise (depends on the capabilities of
         * the SceneViewer).
         */
        bool init (rw::graphics::SceneViewer::Ptr drawer);

        /**
         * @brief set the maximum depth that is percieved by this frame grabber.
         * If min and max depth are too far apart the resolution of the depth
         * perception will become bad. Hence keep the range realistic.
         * @param depth [in] max depth
         */
        void setMaxDepth (double depth);

        /**
         * @brief set the minimum depth that is percieved by this frame grabber.
         * If min and max depth are too far apart the resolution of the depth
         * perception will become bad. Hence keep the range realistic.
         * @param depth [in] min depth
         */
        void setMinDepth (double depth);

        //! @copydoc FrameGrabber::grab
        void grab (rw::core::Ptr<rw::kinematics::Frame> frame, const rw::kinematics::State& state);

        //! @copydoc FrameGrabber25D::getMaxDepth()
        double getMaxDepth () { return _maxDepth; };

        //! @copydoc FrameGrabber25D::getMinDepth()
        double getMinDepth () { return _minDepth; };

        /**
         * @copydoc FrameGrabber25D::getFieldOfViewY()
         */
        virtual double getFieldOfViewY ();

      private:
        double _fieldOfView;    // in the y-axis
        rw::graphics::SceneViewer::Ptr _drawer;
        rw::math::Transform3D< double > _perspTrans;
        rw::graphics::SceneViewer::View::Ptr _view;

        double _minDepth, _maxDepth;
        std::vector< float > _depthData;
    };

    /* @} */
}}    // namespace rwlibs::simulation

#endif    // end include guard
