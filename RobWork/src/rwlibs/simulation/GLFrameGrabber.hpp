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

#ifndef RWLIBS_SIMULATION_CAMERA_GLFRAMEGRAPPER_HPP
#define RWLIBS_SIMULATION_CAMERA_GLFRAMEGRAPPER_HPP

/**
 * @file GLFrameGrabber.hpp
 */
#if !defined(SWIG)
#include <rwlibs/simulation/FrameGrabber.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/graphics/SceneViewer.hpp>
#include <rw/math/Transform3D.hpp>
#endif 

namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rwlibs { namespace simulation {
    /** @addtogroup simulation */
    /* @{ */

    /**
     * @brief An implementation of the FrameGrabber interface. The GLFrameGrabber
     * grabs images from a OpenGL scene using a simple pinhole camera model.
     *
     * a framethe opengl rendering to
     * take pictures of the scene.

     * The most basic parameter of a camera is its Field of view. This
     * can be used as an initial camera model. Field of view can be
     * calculated from the focal length and the size of the CCD
     * typically (1/2, 1/3, 1/4) inch.
     * If a more realistic camera model is
     * required the perspective transform of a specific camera can be added
     */
    class GLFrameGrabber : public FrameGrabber
    {
      public:
        //! @brief Smart pointer type for GLFrameGrabber.
        typedef rw::core::Ptr< GLFrameGrabber > Ptr;

        /**
         * @brief constructor
         * @param width [in] width of image
         * @param height [in] height of image
         * @param fov [in] the vertical field of view angle in degree
         * @param near [in] the minimum depth of camera.
         * @param far [in] the maximum depth of camera.
         */
        GLFrameGrabber (int width, int height, double fov, double near = 0.1, double far = 10.0);

        /**
         * @brief destructor
         */
        virtual ~GLFrameGrabber ();

        void resize (int width, int height);

        void resize (int width, int height, rw::sensor::Image::ColorCode colorCode);

        /**
         * @brief initialize the grabber with a scene viewer. This registers the grabber
         * as a camera in the scene and enables rendering.
         * @param drawer [in] the scene viewer
         * @return true if initialization succeeded, false otherwise (depends on the capabilities of
         * the SceneViewer).
         */
        bool init (rw::core::Ptr<rw::graphics::SceneViewer> drawer);

        //! @copydoc FrameGrabber::grab
        virtual void grab (rw::core::Ptr<rw::kinematics::Frame> frame, const rw::kinematics::State& state);

      private:
        double _fieldOfView;    // in the y-axis
        rw::graphics::SceneViewer::Ptr _drawer;
        rw::math::Transform3D< double > _perspTrans;
        rw::graphics::SceneViewer::View::Ptr _view;
        double _near, _far;
    };

    /* @} */
}}    // namespace rwlibs::simulation

#endif    // end include guard
