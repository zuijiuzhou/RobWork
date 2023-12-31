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

#include "WorkCellScene.hpp"

#include <rw/core/macros.hpp>
#include <rw/graphics/DrawableNode.hpp>
#include <rw/graphics/DrawableNodeClone.hpp>
#include <rw/graphics/Render.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rw/kinematics/State.hpp>
#include <rw/models/DeformableObject.hpp>
#include <rw/models/WorkCell.hpp>
#include <rw/graphics/Model3D.hpp>

#include <boost/bind/bind.hpp>
#include <stack>
#include <vector>

namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

using namespace rw::core;
using namespace rw::geometry;
using namespace rw::graphics;
using namespace rw::kinematics;
using namespace rw::math;
using namespace rw::models;
using namespace rw::sensor;

//----------------------------------------------------------------------------
namespace {
struct FindDrawableVisitor
{
    SceneGraph::NodeVisitor functor;

    FindDrawableVisitor (std::string name, bool findall) : _name (name), _findall (findall)
    {
        functor = boost::ref (*this);
    }

    FindDrawableVisitor (DrawableNode::Ptr d, bool findall) : _findall (findall), _drawable (d)
    {
        functor = boost::ref (*this);
    }

    bool operator() (SceneNode::Ptr& child, SceneNode::Ptr& parent)
    {
        if (child->asDrawableNode ()) {
            bool found = false;
            if (_drawable) {
                // we search for this specific drawable
                if (_drawable == child)
                    found = true;
            }
            else if (child->getName () == _name) {
                found = true;
            }
            DrawableNode::Ptr d = child.cast< DrawableNode > ();
            if (d != NULL && found) {
                if (_findall) {
                    _dnodes.push_back (d);
                    _pnodes.push_back (parent);
                }
                else {
                    _dnode = d;
                    _pnode = parent;
                    return true;
                }
            }
        }
        return false;
    }

    DrawableNode::Ptr _dnode;
    SceneNode::Ptr _pnode;
    std::vector< DrawableNode::Ptr > _dnodes;
    std::vector< SceneNode::Ptr > _pnodes;
    std::string _name;
    bool _findall;
    DrawableNode::Ptr _drawable;    // the one to match
};

struct StaticFilter
{
    SceneGraph::NodeFilter functor;
    StaticFilter (bool retValue) : _retvalue (retValue) { functor = boost::ref (*this); }
    bool operator() (const SceneNode::Ptr&) const { return _retvalue; }
    const bool _retvalue;
};
}    // namespace

//----------------------------------------------------------------------------

// Struct used for transfer of visual state for frames when the same workcell is reloaded
// Frame-level values will override the values for individual drawables, but only if the first value
// of the following pairs are set to true (only when that particular value is set by the user).
struct WorkCellScene::FrameVisualState
{
    FrameVisualState () :
        visible (false, true), highlighted (false, false), alpha (false, 1.0),
        frameAxisVisible (false, false), dtype (false, DrawableNode::SOLID), dmask (false, 0),
        frameLabelVisible (false, false)
    {}
    std::pair< bool, bool > visible;
    std::pair< bool, bool > highlighted;
    std::pair< bool, double > alpha;
    std::pair< bool, bool > frameAxisVisible;
    std::pair< bool, DrawableNode::DrawType > dtype;
    std::pair< bool, unsigned int > dmask;
    std::pair< bool, bool > frameLabelVisible;
};

WorkCellScene::WorkCellScene (SceneGraph::Ptr scene) :
    _scene (scene), _fk (NULL), _worldNode (scene->makeGroupNode ("World"))
{
    _scene->addChild (_worldNode, _scene->getRoot ());
    setWorkCell (NULL);
}

WorkCellScene::~WorkCellScene ()
{
    clearCache ();
}

void WorkCellScene::clearCache ()
{
    _frameNodeMap.clear ();
}

void WorkCellScene::draw (SceneGraph::RenderInfo& info)
{
    _scene->draw (info);
}

void WorkCellScene::workCellChangedListener (int)
{
    State state = _wc->getDefaultState ();
    updateSceneGraph (state);
}

void WorkCellScene::setWorkCell (rw::models::WorkCell::Ptr wc)
{
    if (_wc == wc)
        return;

    // if the workcell name matches the old one then see if we can transfer states of frames to the
    // next workcell
    std::map< std::string, FrameVisualState > fnameToStateMap;
    if (wc != NULL && _wc != NULL && wc->getName () == _wc->getName ()) {
        typedef std::pair< const Frame* const, FrameVisualState > StateMapData;
        for (StateMapData& data : _frameStateMap) {
            fnameToStateMap[data.first->getName ()] = data.second;
        }
    }

    // Unlink nodes in existing SceneGraph, so they can be destructed.
    if (!_wc.isNull ()) {
        const State state = _wc->getDefaultState ();
        std::stack< Frame* > frames;
        frames.push (_wc->getWorldFrame ());

        while (!frames.empty ()) {
            rw::core::Ptr<Frame> frame = frames.top ();
            frames.pop ();
            FrameNodeMap::iterator it = _frameNodeMap.find (frame.get());
            if (it != _frameNodeMap.end ()) {
                GroupNode::Ptr parentNode = _frameNodeMap[frame->getParent (state)];
                parentNode->removeChild (it->second);
                _scene->removeDrawables (it->second);
            }
            Frame::iterator_pair iter = frame->getChildren (state);
            for (; iter.first != iter.second; ++iter.first) {
                rw::core::Ptr<Frame> child = &(*iter.first);
                frames.push (child.get());
            }
        }
    }

    _frameStateMap.clear ();
    _frameDrawableMap.clear ();
    _nodeFrameMap.clear ();
    _frameNodeMap.clear ();
    _wc = wc;
    _scene->getRoot ()->removeChild (_worldNode);

    if (wc != NULL) {
        _wc->workCellChangedEvent ().add (
            boost::bind (&WorkCellScene::workCellChangedListener, this, boost::arg< 1 > ()), this);
        State state = _wc->getDefaultState ();
        updateSceneGraph (state);
    }
    else {
        // std::cout << "************************* CLEAR *****************************" <<
        // std::endl;
        _worldNode = _scene->makeGroupNode ("World");
        _scene->addChild (_worldNode, _scene->getRoot ());
    }

    if (wc != NULL) {
        typedef std::pair< std::string, FrameVisualState > StateStringMapData;
        for (StateStringMapData data : fnameToStateMap) {
            rw::core::Ptr<Frame> frame = _wc->findFrame (data.first);
            if (frame != NULL) {
                if (data.second.visible.first)
                    setVisible (data.second.visible.second, frame);
                if (data.second.frameAxisVisible.first)
                    setFrameAxisVisible (data.second.frameAxisVisible.second, frame);
                if (data.second.highlighted.first)
                    setHighlighted (data.second.highlighted.second, frame);
                if (data.second.alpha.first)
                    setTransparency (data.second.alpha.second, frame);
                if (data.second.dtype.first)
                    setDrawMask (data.second.dmask.second, frame);
                if (data.second.dtype.first)
                    setDrawType (data.second.dtype.second, frame);
                if (data.second.frameLabelVisible.first)
                    setFrameLabelVisible (data.second.frameLabelVisible.second, frame);
            }
        }
    }
}

rw::models::WorkCell::Ptr WorkCellScene::getWorkCell ()
{
    return _wc;
}

void WorkCellScene::setState (const State& state)
{
    _fk.reset (state);

    // iterate through all frame-node pairs and set the node transformations accordingly
    for (FrameNodeMap::value_type data : _frameNodeMap) {
        if ((data.first != NULL) && (data.second != NULL)) {
            // 1. we update the transform of each GroupNode
            data.second->setTransform (data.first->getTransform (state));

            // 2. also make sure that all parent relationships are updated
            if (data.first != _wc->getWorldFrame () && Kinematics::isDAF (data.first)) {
                rw::core::Ptr<const Frame> parent = data.first->getParent (state);
                if (!data.second->hasParent (_frameNodeMap[parent.get()])) {
                    // 1. we remove the child from its parent
                    std::list< SceneNode::Ptr > pnodes = data.second->_parentNodes;
                    for (SceneNode::Ptr parentNode : pnodes) {
                        if (parentNode->asGroupNode ()) {
                            parentNode->asGroupNode ()->removeChild (data.second);
                        }
                    }
                    // 2. and add the daf to its new parent
                    _scene->addChild (data.second, _frameNodeMap[parent.get()]);
                }
            }
        }
    }

    // also iterate through all deformable objects and update their geometries
    typedef std::map< DeformableObject::Ptr, std::vector< Model3D::Ptr > > DeformObjectMap;
    for (DeformObjectMap::value_type data : _deformableObjectsMap) {
        for (Model3D::Ptr model : data.second) {
            data.first->update (model, state);
        }
    }
}

GroupNode::Ptr WorkCellScene::getWorldNode ()
{
    return _worldNode;
}

void WorkCellScene::updateSceneGraph (State& state)
{
    // here we find all drawables that belong to frames and order them according to translucency
    _fk.reset (state);
    // first check that the WORLD frame is in the scene, if its not add it
    _frameNodeMap[NULL]               = _scene->getRoot ();    // for world frame parent
    _nodeFrameMap[_scene->getRoot ()] = NULL;
    RW_ASSERT (_scene->getRoot () != NULL);
    std::stack< Frame* > frames;
    std::set< Frame* > exists;
    frames.push (_wc->getWorldFrame ());

    while (!frames.empty ()) {
        rw::core::Ptr<Frame> frame = frames.top ();
        exists.insert (frame.get());
        frames.pop ();
        // std::cout << "Frame: " << frame->getName() << std::endl;
        // make sure that the frame is in the scene
        if (_frameNodeMap.find (frame.get()) == _frameNodeMap.end ()) {
            GroupNode::Ptr parentNode = _frameNodeMap[frame->getParent (state)];
            // frame is not in the scene, add it.
            GroupNode::Ptr node = _scene->makeGroupNode (frame->getName ());
            _scene->addChild (node, parentNode);
            _frameNodeMap[frame.get()] = node;
            _nodeFrameMap[node]  = frame.get();
        }

        // the frame is there, check that the parent relationship is correct
        GroupNode::Ptr node       = _frameNodeMap[frame.get()];
        _nodeFrameMap[node]       = frame.get();
        GroupNode::Ptr parentNode = _frameNodeMap[frame->getParent (state)];

        if (node == NULL) {
            RW_WARN ("Node is null!");
            continue;
        }
        if (parentNode == NULL) {
            RW_WARN ("ParentNode is null!");
            continue;
        }

        // now for each DrawableInfo on frame check that they are on the frame

        // The information of drawables is located in SceneDescriptor and "model::Object"s
        // first check if there is any object with a frame "frame" in it
        std::vector< Object::Ptr > objs = _wc->getObjects ();
        State state                     = _wc->getDefaultState ();

        for (Object::Ptr obj : objs) {
            // just test if this object has anything to do with the frame
            if (obj->getBase () != frame)
                continue;

            _frameDrawableMap[frame.get()].clear ();

            // check if obj is deformable object
            if (DeformableObject::Ptr dobj = obj.cast< DeformableObject > ()) {
                // make deep copy of the models.
                std::vector< Model3D::Ptr > models = dobj->getModels (state);
                // update the models with current state
                _deformableObjectsMap[dobj] = models;
                for (Model3D::Ptr model : models) {
                    DrawableNode::Ptr dnode =
                        _scene->makeDrawable (model->getName (), model, model->getMask ());
                    _scene->addChild (dnode, node);
                    _frameDrawableMap[frame.get()].push_back (dnode);
                }
            }
            else {
                // the collision info is the geometry and visualization is Model3D
                // in case no models are available the collision geometry will be used
                std::vector< Geometry::Ptr > geoms = obj->getGeometry (state);
                for (Geometry::Ptr geom : geoms) {
                    if (geom->getFrame () != frame)
                        continue;

                    DrawableGeometryNode::Ptr dnode =
                        _scene->makeDrawable (geom->getName (), geom, geom->getMask ());
                    _scene->addChild (dnode, node);
                    _frameDrawableMap[frame.get()].push_back (dnode);
                }

                for (Model3D::Ptr geomodel : obj->getModels ()) {
                    Model3D::Ptr model;
                    model = geomodel;
                    DrawableNode::Ptr dnode =
                        _scene->makeDrawable (model->getName (), model, model->getMask ());
                    _scene->addChild (dnode, node);
                    _frameDrawableMap[frame.get()].push_back (dnode);
                }
            }
        }

        Frame::iterator_pair iter = frame->getChildren (state);
        for (; iter.first != iter.second; ++iter.first) {
            rw::core::Ptr<Frame> child = &(*iter.first);
            frames.push (child.get());
        }
    }

    _worldNode = _frameNodeMap[_wc->getWorldFrame ()];

    // Removing non-exsisting frames from NodeFrameMap
    std::set< GroupNode::Ptr > doesntExist;
    for (NodeFrameMap::iterator elem = _nodeFrameMap.begin (); elem != _nodeFrameMap.end ();) {
        if (!(exists.find (elem->second) != exists.end () || elem->second == NULL)) {
            doesntExist.insert (elem->first);
            elem = _nodeFrameMap.erase (elem);
        }
        else {
            elem++;
        }
    }
    // Removing non-exsisting nodes from FrameNodeMap
    typedef std::map< const rw::kinematics::Frame*, GroupNode::Ptr > FrameNodeMap;
    for (FrameNodeMap::iterator elem = _frameNodeMap.begin (); elem != _frameNodeMap.end ();) {
        if (doesntExist.find (elem->second) != doesntExist.end ()) {
            const GroupNode::Ptr gnode = elem->second;
            elem                       = _frameNodeMap.erase (elem);
            if (gnode->_childNodes.size () != 0) {
                RW_WARN ("The deleted frame has undeleted children");
            }
            // Removing SceneNode parents
            std::list< SceneNode::Ptr > parent = gnode->_parentNodes;
            while (parent.size () > 0) {
                for (auto par : _frameNodeMap) {
                    if (par.second->getName () == parent.back ()->getName ()) {
                        par.second->removeChild (gnode);
                    }
                }
                parent.pop_back ();
            }
        }
        else {
            elem++;
        }
    }
}

void WorkCellScene::setVisible (bool visible, const rw::core::Ptr< Frame> f)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        return;
    _frameStateMap[f.get()].visible.first  = true;
    _frameStateMap[f.get()].visible.second = visible;
    for (SceneNode::Ptr& d : _frameNodeMap[f.get()]->_childNodes) {
        if (DrawableNode* dnode = d->asDrawableNode ())
            dnode->setVisible (visible);
    }
}

bool WorkCellScene::isVisible (const rw::core::Ptr< Frame> f) const
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        return false;
    const std::map< const Frame*, FrameVisualState >::const_iterator it = _frameStateMap.find (f.get());
    if (it != _frameStateMap.end ())
        return it->second.visible.second;
    else
        return false;
}

void WorkCellScene::setHighlighted (bool highlighted, const rw::core::Ptr< Frame> f)
{
    if (_frameDrawableMap.find (f.get()) == _frameDrawableMap.end ()) {
        return;
    }

    _frameStateMap[f.get()].highlighted.first  = true;
    _frameStateMap[f.get()].highlighted.second = highlighted;

    for (DrawableNode::Ptr& d : _frameDrawableMap[f.get()]) {
        d->setHighlighted (highlighted);
    }
}

bool WorkCellScene::isHighlighted (const rw::core::Ptr< Frame> f) const
{
    const std::map< const Frame*, FrameVisualState >::const_iterator it = _frameStateMap.find (f.get());
    if (it != _frameStateMap.end ())
        return it->second.highlighted.second;
    else
        return false;
}

void WorkCellScene::setFrameAxisVisible (bool visible, rw::core::Ptr< Frame> f, double size)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ()) {
        return;
    }
    _frameStateMap[f.get()].frameAxisVisible.first  = true;
    _frameStateMap[f.get()].frameAxisVisible.second = visible;
    GroupNode::Ptr node                       = _frameNodeMap[f.get()];
    if (visible) {
        if (node->hasChild ("FrameAxis")) {
            // remove current frameaxis
            removeDrawable ("FrameAxis", f);
        }

        // Add a frame axis of specified size
        DrawableNode::Ptr frameAxisNode =
            _scene->makeDrawableFrameAxis ("FrameAxis", size, DrawableNode::Virtual);
        if (!frameAxisNode.isNull ()) {
            frameAxisNode->setName ("FrameAxis");
            addDrawable (frameAxisNode, f);
        }
        //_frameDrawableMap[f].push_back(dnode);
        // node->addChild( dnode );
        _scene->update ();
    }
    else if (!visible && node->hasChild ("FrameAxis")) {
        // remove leaf
        // node->removeChild( "FrameAxis" );
        removeDrawable ("FrameAxis", f);
        _scene->update ();
    }
}

bool WorkCellScene::isFrameAxisVisible (const rw::core::Ptr< Frame> f) const
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        RW_THROW ("Frame is not in the scene!");
    const std::map< const Frame*, FrameVisualState >::const_iterator it = _frameStateMap.find (f.get());
    if (it != _frameStateMap.end ())
        return it->second.frameAxisVisible.second;
    else
        return false;
}

void WorkCellScene::setFrameLabelVisible (bool visible, Frame::Ptr f)
{
    if (_frameNodeMap.find (f.get ()) == _frameNodeMap.end ()) {
        return;
    }
    _frameStateMap[f.get ()].frameLabelVisible.first  = true;
    _frameStateMap[f.get ()].frameLabelVisible.second = visible;
    GroupNode::Ptr node                               = _frameNodeMap[f.get ()];
    if (visible) {
        if (node->hasChild ("FrameLabel")) {
            // remove current frameLabel
            removeDrawable ("FrameLabel", f.get ());
        }

        // Add a frame axis of specified size
        DrawableNode::Ptr frameLabelNode = _scene->makeDrawable ("FrameLabel", f->getName (), f);
        if (!frameLabelNode.isNull ()) {
            frameLabelNode->setName ("FrameLabel");
            addDrawable (frameLabelNode, f.get ());
        }

        _scene->update ();
    }
    else if (!visible && node->hasChild ("FrameLabel")) {
        removeDrawable ("FrameLabel", f.get ());
        _scene->update ();
    }
}

bool WorkCellScene::isFrameLabelVisible (const Frame::Ptr f) const
{
    if (_frameNodeMap.find (f.get ()) == _frameNodeMap.end ()) {
        RW_THROW ("Frame is not in the scene!");
    }
    const std::map< const Frame*, FrameVisualState >::const_iterator it =
        _frameStateMap.find (f.get ());
    if (it != _frameStateMap.end ()) {
        return it->second.frameLabelVisible.second;
    }

    return false;
}

void WorkCellScene::setDrawType (DrawableNode::DrawType type, const rw::core::Ptr< Frame> f)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        return;
    _frameStateMap[f.get()].dtype.first  = true;
    _frameStateMap[f.get()].dtype.second = type;
    for (DrawableNode::Ptr& d : _frameDrawableMap[f.get()]) {
        d->setDrawType (type);
    }
}

DrawableNode::DrawType WorkCellScene::getDrawType (const rw::core::Ptr< Frame> f) const
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        RW_THROW ("Frame is not in the scene!");
    const std::map< const Frame*, FrameVisualState >::const_iterator it = _frameStateMap.find (f.get());
    if (it != _frameStateMap.end ())
        return it->second.dtype.second;
    else
        return DrawableNode::SOLID;
}

void WorkCellScene::setDrawMask (unsigned int mask, const rw::core::Ptr< Frame> f)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        RW_THROW ("Frame is not in the scene!");
    _frameStateMap[f.get()].dmask.first  = true;
    _frameStateMap[f.get()].dmask.second = mask;
    for (DrawableNode::Ptr& d : _frameDrawableMap[f.get()]) {
        d->setMask (mask);
    }
}

unsigned int WorkCellScene::getDrawMask (const rw::core::Ptr< Frame> f) const
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        RW_THROW ("Frame is not in the scene!");
    const std::map< const Frame*, FrameVisualState >::const_iterator it = _frameStateMap.find (f.get());
    if (it != _frameStateMap.end ())
        return it->second.dmask.second;
    else
        return DrawableNode::SOLID;
}

void WorkCellScene::setTransparency (double alpha, const rw::core::Ptr< Frame> f)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        return;
    _frameStateMap[f.get()].alpha.first  = true;
    _frameStateMap[f.get()].alpha.second = alpha;
    for (DrawableNode::Ptr& d : _frameDrawableMap[f.get()]) {
        d->setTransparency ((float) alpha);
    }
}

void WorkCellScene::addDrawable (DrawableNode::Ptr drawable, rw::core::Ptr< Frame> frame)
{
    if (_wc == NULL)
        RW_THROW (
            "Scene is not initialized with WorkCell yet! Drawable cannot be attached to Frame.");
    // add frame to frame map
    _frameDrawableMap[frame.get()].push_back (drawable);

    // get or create the
    if (_frameNodeMap.find (frame.get()) == _frameNodeMap.end ()) {
        // the frame is not in the scene, add it
        State state = _wc->getDefaultState ();

        rw::core::Ptr<Frame> p = frame->getParent (state);

        std::stack< Frame* > frames;
        frames.push (frame.get());
        while (_frameNodeMap.find (p.get()) == _frameNodeMap.end ()) {
            frames.push (p.get());
            p = p->getParent (state);
            RW_ASSERT (p != NULL);
        }

        while (!frames.empty ()) {
            rw::core::Ptr<Frame> f = frames.top ();
            frames.pop ();

            // now f's parent is supposed to have a node allready
            RW_ASSERT (_frameNodeMap.find (f->getParent (state)) != _frameNodeMap.end ());

            GroupNode::Ptr pnode = _frameNodeMap[f->getParent (state)];
            GroupNode::Ptr child = _scene->makeGroupNode (f->getName ());
            GroupNode::addChild (child, pnode);
            _frameNodeMap[f.get()]     = child;
            _nodeFrameMap[child] = f.get();
        }
    }

    _frameNodeMap[frame.get()]->addChild (drawable);
}

DrawableNode::Ptr WorkCellScene::addDrawable (const std::string& filename, rw::core::Ptr< Frame> frame, int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (filename, dmask);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addFrameAxis (const std::string& name, double size, rw::core::Ptr< Frame> frame,
                                               int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawableFrameAxis (name, size, dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableGeometryNode::Ptr WorkCellScene::addGeometry (const std::string& name, Geometry::Ptr geom,
                                                      rw::core::Ptr<Frame> frame, int dmask)
{
    DrawableGeometryNode::Ptr drawable = _scene->makeDrawable (name, geom);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addModel3D (const std::string& name, Model3D::Ptr model,
                                             rw::core::Ptr<Frame> frame, int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (name, model);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addImage (const std::string& name, const class Image& img,
                                           rw::core::Ptr<Frame> frame, int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (name, img);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addScan (const std::string& name,
                                          const class rw::geometry::PointCloud& scan, rw::core::Ptr< Frame> frame,
                                          int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (name, scan);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableGeometryNode::Ptr WorkCellScene::addLines (const std::string& name,
                                                   const std::vector< Line >& lines, rw::core::Ptr< Frame> frame,
                                                   int dmask)
{
    DrawableGeometryNode::Ptr drawable = _scene->makeDrawable (name, lines);
    if (drawable == NULL)
        return drawable;
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addText (const std::string& name, const std::string& text,
                                          Frame::Ptr frame, int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (name, text, frame);
    drawable->setMask (dmask);
    addDrawable (drawable, frame.get ());
    return drawable;
}

DrawableNode::Ptr WorkCellScene::addRender (const std::string& name, Render::Ptr render,
                                            rw::core::Ptr<Frame> frame, int dmask)
{
    DrawableNode::Ptr drawable = _scene->makeDrawable (name, render);
    drawable->setMask (dmask);
    addDrawable (drawable, frame);
    return drawable;
}

std::vector< DrawableNode::Ptr > WorkCellScene::getDrawables ()
{
    return _scene->getDrawables ();
}

std::vector< DrawableNode::Ptr > WorkCellScene::getDrawables (const rw::core::Ptr< Frame> f) const
{
    std::map< const Frame*, std::vector< DrawableNode::Ptr > >::const_iterator it =
        _frameDrawableMap.find (f.get());
    if (it == _frameDrawableMap.end ())
        return std::vector< DrawableNode::Ptr > ();
    else
        return it->second;
}

std::vector< DrawableNode::Ptr > WorkCellScene::getDrawablesRec (rw::core::Ptr<Frame> f, State&)
{
    if (_frameNodeMap.find (f.get()) == _frameNodeMap.end ())
        return std::vector< DrawableNode::Ptr > ();
    return _scene->getDrawablesRec (_frameNodeMap[f.get()]);
}

DrawableNode::Ptr WorkCellScene::findDrawable (const std::string& name)
{
    return _scene->findDrawable (name);
}

DrawableNode::Ptr WorkCellScene::findDrawable (const std::string& name, const rw::core::Ptr< Frame> f)
{
    const FrameNodeMap::const_iterator it = _frameNodeMap.find (f.get());
    if (it == _frameNodeMap.end ())
        return NULL;
    else
        return _scene->findDrawable (name, it->second);
}

std::vector< DrawableNode::Ptr > WorkCellScene::findDrawables (const std::string& name)
{
    return _scene->findDrawables (name);
}

bool WorkCellScene::removeDrawable (DrawableNode::Ptr drawable)
{
    const std::list< SceneNode::Ptr > parents = drawable->_parentNodes;
    for (std::list< SceneNode::Ptr >::const_iterator itP = parents.begin (); itP != parents.end ();
         itP++) {
        const GroupNode::Ptr gn = (*itP)->asGroupNode ();
        if (gn.isNull ())
            continue;
        const NodeFrameMap::const_iterator itF = _nodeFrameMap.find (gn);
        if (itF == _nodeFrameMap.end ())
            continue;
        std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
            _frameDrawableMap.find (itF->second);
        if (itD == _frameDrawableMap.end ())
            continue;
        std::vector< DrawableNode::Ptr >& drawables = itD->second;
        for (std::vector< DrawableNode::Ptr >::iterator itDrawables = drawables.begin ();
             itDrawables != drawables.end ();
             itDrawables++) {
            if ((*itDrawables) == drawable) {
                drawables.erase (itDrawables);
                break;
            }
        }
    }

    return _scene->removeDrawable (drawable);
}

bool WorkCellScene::removeDrawables (const rw::core::Ptr< Frame> f)
{
    std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
        _frameDrawableMap.find (f.get());
    if (itD != _frameDrawableMap.end ()) {
        std::vector< DrawableNode::Ptr >& drawables = itD->second;
        drawables.clear ();
    }

    const FrameNodeMap::const_iterator it = _frameNodeMap.find (f.get());
    if (it == _frameNodeMap.end ())
        return false;
    else
        return _scene->removeDrawables (it->second);
}

bool WorkCellScene::removeDrawable (const std::string& name)
{
    FindDrawableVisitor visitor (name, false);
    SceneNode::Ptr root = _worldNode.cast< SceneNode > ();
    _scene->traverse (root, visitor.functor, StaticFilter (false).functor);
    if (visitor._dnode.isNull ())
        return true;
    if (GroupNode* const gn = visitor._pnode->asGroupNode ()) {
        const NodeFrameMap::const_iterator itF = _nodeFrameMap.find (gn);
        if (itF != _nodeFrameMap.end ()) {
            std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
                _frameDrawableMap.find (itF->second);
            if (itD != _frameDrawableMap.end ()) {
                std::vector< DrawableNode::Ptr >& drawables = itD->second;
                for (std::vector< DrawableNode::Ptr >::iterator itDrawables = drawables.begin ();
                     itDrawables != drawables.end ();
                     itDrawables++) {
                    if ((*itDrawables)->getName () == name) {
                        drawables.erase (itDrawables);
                        break;
                    }
                }
            }
        }
    }

    return _scene->removeDrawable (name);
}

bool WorkCellScene::removeDrawables (const std::string& name)
{
    const std::vector< DrawableNode::Ptr > drawables = _scene->findDrawables (name);
    for (std::vector< DrawableNode::Ptr >::const_iterator it = drawables.begin ();
         it != drawables.end ();
         it++) {
        const DrawableNode::Ptr d                 = *it;
        const std::list< SceneNode::Ptr > parents = d->_parentNodes;
        for (std::list< SceneNode::Ptr >::const_iterator itP = parents.begin ();
             itP != parents.end ();
             itP++) {
            const GroupNode::Ptr gn = (*itP)->asGroupNode ();
            if (gn.isNull ())
                continue;
            const NodeFrameMap::const_iterator itF = _nodeFrameMap.find (gn);
            if (itF == _nodeFrameMap.end ())
                continue;
            std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
                _frameDrawableMap.find (itF->second);
            if (itD == _frameDrawableMap.end ())
                continue;
            std::vector< DrawableNode::Ptr >& drawables = itD->second;
            for (std::vector< DrawableNode::Ptr >::iterator itDrawables = drawables.begin ();
                 itDrawables != drawables.end ();) {
                if ((*itDrawables)->getName () == name) {
                    itDrawables = drawables.erase (itDrawables);
                }
                else {
                    itDrawables++;
                }
            }
        }
    }

    return _scene->removeDrawables (name);
}

bool WorkCellScene::removeDrawable (DrawableNode::Ptr drawable, const rw::core::Ptr< Frame> f)
{
    std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
        _frameDrawableMap.find (f.get());
    if (itD != _frameDrawableMap.end ()) {
        std::vector< DrawableNode::Ptr >& drawables = itD->second;
        for (std::vector< DrawableNode::Ptr >::iterator it = drawables.begin ();
             it != drawables.end ();
             it++) {
            if ((*it) == drawable) {
                drawables.erase (it);
                break;
            }
        }
    }

    const FrameNodeMap::const_iterator it = _frameNodeMap.find (f.get());
    if (it == _frameNodeMap.end ())
        return false;
    else
        return _scene->removeDrawable (drawable, it->second);
}

bool WorkCellScene::removeDrawable (const std::string& name, const rw::core::Ptr< Frame> f)
{
    std::map< const Frame*, std::vector< DrawableNode::Ptr > >::iterator itD =
        _frameDrawableMap.find (f.get());
    if (itD != _frameDrawableMap.end ()) {
        std::vector< DrawableNode::Ptr >& drawables = itD->second;
        for (std::vector< DrawableNode::Ptr >::iterator it = drawables.begin ();
             it != drawables.end ();
             it++) {
            if ((*it)->getName () == name) {
                drawables.erase (it);
                break;
            }
        }
    }

    const FrameNodeMap::const_iterator it = _frameNodeMap.find (f.get());
    if (it == _frameNodeMap.end ())
        return false;
    else
        return _scene->removeChild (name, it->second);
}

Frame* WorkCellScene::getFrame (DrawableNode::Ptr d) const
{
    GroupNode::Ptr gn = d->_parentNodes.front ().cast< GroupNode > ();
    if (gn == NULL) {
        RW_WARN ("Group Node is NULL");
        return NULL;
    }
    // std::cout << "GroupeNode: " << gn->getName() << std::endl;
    const NodeFrameMap::const_iterator it = _nodeFrameMap.find (gn);
    if (it != _nodeFrameMap.end ())
        return it->second;
    else
        return NULL;
}

GroupNode::Ptr WorkCellScene::getNode (const rw::core::Ptr< Frame> frame) const
{
    const FrameNodeMap::const_iterator it = _frameNodeMap.find (frame.get());
    if (it != _frameNodeMap.end ())
        return it->second;
    else
        return NULL;
}
