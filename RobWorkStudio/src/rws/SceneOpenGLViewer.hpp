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

#ifndef QTGUI_SceneOpenGLViewer_HPP
#define QTGUI_SceneOpenGLViewer_HPP

#ifdef __WIN32
#include <windows.h>
#endif

#include "CameraController.hpp"
#include "SceneViewerWidget.hpp"

#include <rw/core/Property.hpp>
#include <rw/core/PropertyMap.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/graphics/SceneCamera.hpp>
#include <rw/math/Vector3D.hpp>
#include <rwlibs/opengl/SceneOpenGL.hpp>

#include <QOpenGLWidget>
#include <QObject>
#include <vector>

namespace rw { namespace graphics {
    class Render;
}}    // namespace rw::graphics
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics
namespace rw { namespace models {
    class WorkCell;
}}    // namespace rw::models
namespace rw { namespace core {
    class PropertyBase;
}}    // namespace rw::core
namespace rwlibs { namespace opengl {
    class SceneOpenGL;
}}    // namespace rwlibs::opengl

class QMouseEvent;

namespace rws {
/**
 * @brief Class representing an OpenGL based QT Widget for 3D visualization of the SceneGraph
 * SceneOpenGL.
 *
 * One of the main responsibilities of the view is to define an opengl context in which
 * the scenegraph can operate and render.
 *
 * The view will add a "view" camera group and a "view" camera to the scene which it will control
 * through its gui events (mouse and keyboard).
 *
 * Multiple camera
 *
 */
class SceneOpenGLViewer : public QOpenGLWidget, public SceneViewerWidget
{
    Q_OBJECT

  public:
    //! @brief Smart pointer type for SceneOpenGLViewer.
    typedef rw::core::Ptr< SceneOpenGLViewer > Ptr;

    /**
     * @brief Constructor.
     * @param parent [in] the parent widget (the owner of this widget).
     */
    SceneOpenGLViewer (QWidget* parent = 0);

    /**
     * @brief Constructs an OpenGL based QT Widget.
     * @param pmap [in] propertyies for the viewer.
     * @param parent [in] the parent widget (the owner of this widget).
     */
    SceneOpenGLViewer (rw::core::PropertyMap& pmap, QWidget* parent = 0);

    //! @brief Destructor.
    virtual ~SceneOpenGLViewer ();

    // ----------------- SceneViewerWidget interface --------------------
    //! @copydoc SceneViewerWidget::getRenderInfo
    rw::graphics::SceneGraph::RenderInfo& getRenderInfo () { return _renderInfo; }

    //! @copydoc SceneViewerWidget::getPivotDrawable
    rw::graphics::DrawableNode::Ptr getPivotDrawable () { return _pivotDrawable; }

    //! @copydoc SceneViewerWidget::getWidget
    QWidget* getWidget () { return this; }

    // ----------------- SceneViewer interface --------------------
    //! @copydoc rw::graphics::SceneViewer::getScene
    rw::graphics::SceneGraph::Ptr getScene () { return _scene; }

    //! @copydoc rw::graphics::SceneViewer::getPropertyMap
    rw::core::PropertyMap& getPropertyMap () { return _pmap->getValue (); }

    //! @copydoc rw::graphics::SceneViewer::getViewCamera
    virtual rw::graphics::SceneCamera::Ptr getViewCamera () { return _mainCam; }

    //! @copydoc rw::graphics::SceneViewer::setWorldNode
    void setWorldNode (rw::graphics::GroupNode::Ptr wnode);

    //! @copydoc rw::graphics::SceneViewer::getWorldNode
    rw::graphics::GroupNode::Ptr getWorldNode () { return _worldNode; }

    //! @copydoc rw::graphics::SceneViewer::createView
    virtual View::Ptr createView (const std::string& name, bool enableBackground = false);

    //! @copydoc rw::graphics::SceneViewer::getMainView
    virtual View::Ptr getMainView () { return _mainView; }

    //! @copydoc rw::graphics::SceneViewer::destroyView
    virtual void destroyView (View::Ptr view);

    //! @copydoc rw::graphics::SceneViewer::selectView
    virtual void selectView (View::Ptr view);

    //! @copydoc rw::graphics::SceneViewer::getCurrentView
    virtual View::Ptr getCurrentView () { return _currentView; }

    //! @copydoc rw::graphics::SceneViewer::getViews
    virtual std::vector< View::Ptr > getViews () { return _views; }

    //! @copydoc rw::graphics::SceneViewer::updateState
    void updateState (const rw::kinematics::State& state);

    //! @copydoc rw::graphics::SceneViewer::updateView
    void updateView () { QWidget::update (); }

    //! @copydoc rw::graphics::SceneViewer::getViewCenter
    rw::math::Vector3D<> getViewCenter () { return _cameraCtrl->getCenter (); }

    //! @copydoc rw::graphics::SceneViewer::setLogo
    void setLogo (const std::string& string)
    {
        _viewLogo = string;
        update ();
    }

    //! @copydoc rw::graphics::SceneViewer::getLogo
    const std::string& getLogo () const { return _viewLogo; }

    //! @copydoc rw::graphics::SceneViewer::setTransform
    virtual void setTransform (const rw::math::Transform3D<>& t3d)
    {
        _cameraCtrl->setTransform (t3d);
        getViewCamera ()->setTransform (t3d);
        // updateGL();
    }

    //! @copydoc rw::graphics::SceneViewer::pickDrawable(int,int)
    rw::graphics::DrawableNode::Ptr pickDrawable (int x, int y);

    //! @copydoc
    //! rw::graphics::SceneViewer::pickDrawable(rw::graphics::SceneGraph::RenderInfo&,int,int)
    rw::graphics::DrawableNode::Ptr pickDrawable (rw::graphics::SceneGraph::RenderInfo& info, int x,
                                                  int y);

    //! @copydoc rw::graphics::SceneViewer::saveBufferToFile
    void saveBufferToFile (const std::string& filename, const int fillR, const int fillG,
                           const int fillB);

    //! @brief Clears the list of Drawables and WorkCells
    void clear ();

    /**
     * @brief key pressed listener function. Key events in the opengl view
     * will trigger this method.
     * @param e [in] the event.
     */
    void keyPressEvent (QKeyEvent* e);

    /**
     * @brief set the camera view controller.
     * @param camController
     */
    void setCameraController (CameraController::Ptr camController) { _cameraCtrl = camController; }

    /**
     * @brief Returns the camera controller
     * @brief Camera controller
     */
    CameraController::Ptr getCameraController () { return _cameraCtrl; }

    /**
     * @brief listener callback for property changed in getPropertyMap
     * @param base
     */
    void propertyChangedListener (rw::core::PropertyBase* base);

    /**
     * @brief picks the frame that has drawables that intersect the ray cast into the screen from
     * the screen coordinates \b x and \b y.
     * @param x [in] x coordinate
     * @param y [in] y coordinate
     * @return the frame that was selected, Null if no frames where selected.
     */
    rw::kinematics::Frame* pickFrame (int x, int y);

    //! @copydoc rw::graphics::SceneViewer::renderView
    void renderView (View::Ptr view);

  private Q_SLOTS:
    //! @copydoc rw::graphics::SceneViewer::renderView
    void renderViewThreadSafe (View::Ptr view);

  protected:
    //! Overridden from QOpenGLWidget
    void initializeGL ();

    //! Overridden from QOpenGLWidget
    void paintGL ();

    //! Overridden from QOpenGLWidget
    void resizeGL (int width, int height);

    //! Overridden from QOpenGLWidget
    void mouseDoubleClickEvent (QMouseEvent* event);

    //! Overridden from QOpenlGLWidget
    void mousePressEvent (QMouseEvent* event);

    //! Overridden from QOpenGLWidget
    void mouseMoveEvent (QMouseEvent* event);

    //! Overridden from QOpenGLWidget
    void wheelEvent (QWheelEvent* event);

    //! @copydoc rw::graphics::SceneViewer::setWorkCellScene
    void setWorkCellScene (rw::core::Ptr< rw::graphics::WorkCellScene > wcscene)
    {
        _wcscene = wcscene;
    }

    //! @copydoc rw::graphics::SceneViewer::zoom
    void zoom (double amount);

    //! @copydoc rw::graphics::SceneViewer::autoZoom
    void autoZoom ();

  private:
    void init ();

    void propertyUpdated (rw::core::PropertyBase* base);

    void setupCameraView (int camNr, bool setupViewport = true);

  private:
    rwlibs::opengl::SceneOpenGL::Ptr _scene;
    rw::core::Ptr< rw::graphics::WorkCellScene > _wcscene;

    // the main camera which is controlled by the gui
    rw::graphics::SceneCamera::Ptr _mainCam, _backCam, frontCam;
    rw::graphics::CameraGroup::Ptr _mainCamGroup;

    View::Ptr _mainView, _currentView;
    std::vector< View::Ptr > _views;

    rw::core::Ptr< rw::kinematics::State > _state;

    std::string _viewLogo;

    int _width, _height;
    // Background Color definitions

    rw::core::Property< rw::core::PropertyMap >::Ptr _pmap;
    rw::core::Property< bool >::Ptr _viewBackground;
    rw::core::Property< rw::math::Vector3D<> >::Ptr _backgroundColorTop, _backgroundColorBottom;

    CameraController::Ptr _cameraCtrl;
    rw::core::Ptr< rw::graphics::Render > _backgroundRender;
    rw::graphics::DrawableNode::Ptr _backgroundnode;
    rw::graphics::DrawableGeometryNode::Ptr _pivotDrawable;
    rw::graphics::GroupNode::Ptr _worldNode;
    rw::graphics::SceneGraph::RenderInfo _renderInfo;
};

}    // namespace rws

#endif    //#ifndef QTGUI_SceneOpenGLViewer_HPP
