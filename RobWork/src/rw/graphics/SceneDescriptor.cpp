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

#include "SceneDescriptor.hpp"

#include <rw/graphics/DrawableNode.hpp>

using namespace rw::graphics;
using namespace rw::kinematics;
using namespace rw::geometry;
using namespace rw::core;
using namespace rw::sensor;

//----------------------------------------------------------------------------

SceneDescriptor::SceneDescriptor ()
{}

SceneDescriptor::~SceneDescriptor ()
{}

void SceneDescriptor::setVisible (bool visible, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        return;
    _frameStateMap[f.get()].visible = visible;
    for (DrawableProxy::Ptr d : _frameStateMap[f.get()].drawables) {
        d->visible = visible;
    }
}

bool SceneDescriptor::isVisible (rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        return false;
    return _frameStateMap[f.get()].visible;
}

void SceneDescriptor::setHighlighted (bool highlighted, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        return;

    _frameStateMap[f.get()].highlighted = highlighted;

    for (DrawableProxy::Ptr d : _frameStateMap[f.get()].drawables) {
        d->highlighted = highlighted;
    }
}

bool SceneDescriptor::isHighlighted (rw::core::Ptr<rw::kinematics::Frame> f)
{
    return _frameStateMap[f.get()].highlighted;
}

void SceneDescriptor::setFrameAxisVisible (bool visible, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ()) {
        return;
    }
    _frameStateMap[f.get()].frameAxisVisible = visible;
}

bool SceneDescriptor::isFrameAxisVisible (rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        RW_THROW ("Frame is not in the scene!");
    return _frameStateMap[f.get()].frameAxisVisible;
}

void SceneDescriptor::setDrawType (DrawableNode::DrawType type, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        return;
    _frameStateMap[f.get()].dtype = type;
    for (DrawableProxy::Ptr d : _frameStateMap[f.get()].drawables) {
        d->dtype = type;
    }
}

DrawableNode::DrawType SceneDescriptor::getDrawType (rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        RW_THROW ("Frame is not in the scene!");
    return _frameStateMap[f.get()].dtype;
}

void SceneDescriptor::setDrawMask (unsigned int mask, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        RW_THROW ("Frame is not in the scene!");
    _frameStateMap[f.get()].dmask = mask;
    for (DrawableProxy::Ptr d : _frameStateMap[f.get()].drawables) {
        d->dmask = mask;
    }
}

unsigned int SceneDescriptor::getDrawMask (rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        RW_THROW ("Frame is not in the scene!");
    return _frameStateMap[f.get()].dmask;
}

void SceneDescriptor::setTransparency (double alpha, rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameStateMap.find (f.get()) == _frameStateMap.end ())
        return;

    _frameStateMap[f.get()].alpha = alpha;
    for (DrawableProxy::Ptr d : _frameStateMap[f.get()].drawables) {
        d->alpha = alpha;
    }
}

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addDrawable (DrawableNode::Ptr drawable,
                                                                  rw::core::Ptr<rw::kinematics::Frame> frame)
{
    // add frame to frame map
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = drawable->getName ();
    // proxy->dmask = drawable->dmask;
    proxy->dnode = drawable;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr
SceneDescriptor::addDrawable (const std::string& filename, rw::core::Ptr<rw::kinematics::Frame> frame, int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = filename;
    proxy->dmask             = dmask;
    proxy->filename          = filename;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addFrameAxis (const std::string& name,
                                                                   double size,
                                                                   rw::core::Ptr<rw::kinematics::Frame> frame,
                                                                   int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->frameSize         = size;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr
SceneDescriptor::addGeometry (const std::string& name,
                              rw::core::Ptr< class rw::geometry::Geometry > geom,
                              rw::core::Ptr<rw::kinematics::Frame> frame, int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->dmask             = dmask;
    proxy->geom              = geom;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addModel3D (const std::string& name,
                                                                 Model3D::Ptr model,
                                                                 rw::core::Ptr<rw::kinematics::Frame> frame,
                                                                 int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->dmask             = dmask;
    proxy->model             = model;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addImage (const std::string& name,
                                                               rw::sensor::Image::Ptr img,
                                                               rw::core::Ptr<rw::kinematics::Frame> frame,
                                                               int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->dmask             = dmask;
    proxy->img               = img;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addScan (const std::string& name,
                                                              rw::geometry::PointCloud::Ptr scan,
                                                              rw::core::Ptr<rw::kinematics::Frame> frame,
                                                              int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->dmask             = dmask;
    proxy->scan25            = scan;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

/*
SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addLines(const std::string& name,const
std::vector<rw::geometry::Line >& lines, rw::core::Ptr<rw::kinematics::Frame> frame, int dmask){
    DrawableGeometryNode::Ptr drawable = _scene->makeDrawable(name, lines);
    if(drawable==NULL)
        return drawable;
    drawable->setMask(dmask);
    addDrawable(drawable, frame);
    return drawable;
}

*/

SceneDescriptor::DrawableProxy::Ptr SceneDescriptor::addRender (const std::string& name,
                                                                rw::graphics::Render::Ptr render,
                                                                rw::core::Ptr<rw::kinematics::Frame> frame,
                                                                int dmask)
{
    DrawableProxy::Ptr proxy = ownedPtr (new DrawableProxy ());
    proxy->name              = name;
    proxy->dmask             = dmask;
    proxy->render            = render;

    _frameStateMap[frame.get()].drawables.push_back (proxy);
    return proxy;
}

std::vector< SceneDescriptor::DrawableProxy::Ptr >
SceneDescriptor::getDrawables (rw::core::Ptr<rw::kinematics::Frame> f)
{
    return _frameStateMap[f.get()].drawables;
}

/*
void SceneDescriptor::findDrawable(const std::string& name, rw::core::Ptr<rw::kinematics::Frame> f){
    if(_frameStateMap.find(f)==_frameStateMap.end())
        return NULL;
    return _scene->findDrawable(name, _frameStateMap[f]);
}

bool SceneDescriptor::removeDrawable(DrawableNode::Ptr drawable){
    return _scene->removeDrawable(drawable);
}

bool SceneDescriptor::removeDrawables(rw::core::Ptr<rw::kinematics::Frame> f){
    if(_frameStateMap.find(f)==_frameStateMap.end())
        return false;
    return _scene->removeDrawables(_frameStateMap[f]);
}

bool SceneDescriptor::removeDrawable(const std::string& name){
    return _scene->removeDrawable(name);
}

bool SceneDescriptor::removeDrawables(const std::string& name){
    return _scene->removeDrawables(name);
}

bool SceneDescriptor::removeDrawable(DrawableNode::Ptr drawable, rw::core::Ptr<rw::kinematics::Frame> f){
    if(_frameStateMap.find(f)==_frameStateMap.end())
        return false;
    return _scene->removeDrawable(drawable, _frameStateMap[f]);
}

bool SceneDescriptor::removeDrawable(const std::string& name, rw::core::Ptr<rw::kinematics::Frame> f){
    if(_frameStateMap.find(f)==_frameStateMap.end())
        return false;
    return _scene->removeChild(name, _frameStateMap[f]);
}


rw::core::Ptr<rw::kinematics::Frame> SceneDescriptor::getFrame(DrawableNode::Ptr d){
    //std::cout << d->_parentNodes.size() << std::endl;
    GroupNode::Ptr gn = d->_parentNodes.front().cast<GroupNode>();
    if(gn==NULL)
        return NULL;
    return _nodeFrameMap[gn];
}
*/
