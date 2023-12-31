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

#include "Frame.hpp"

#include "Kinematics.hpp"
#include "State.hpp"
#include "TreeState.hpp"
using namespace rw::math;
using namespace rw::kinematics;

Frame::Frame (int dof, const std::string& name) : StateData (dof, name), _parent (NULL)
{}

// Parents.

Frame* Frame::getParent (const State& state)
{
    rw::core::Ptr<Frame>  f1 = getParent ();
    if (f1)
        return f1.get();
    else
        return getDafParent (state);
}

const Frame* Frame::getParent (const State& state) const
{
    rw::core::Ptr<const Frame> f1 = getParent ();
    if (f1)
        return f1.get();
    else
        return getDafParent (state);
}

const Frame* Frame::getDafParent (const State& state) const
{
    return state.getTreeState ().getParent (this);
}

Frame* Frame::getDafParent (const State& state)
{
    return state.getTreeState ().getParent (rw::core::Ptr<Frame>(this));
}

// Children.

Frame::const_iterator_pair Frame::getChildren (const State& state) const
{
    const std::vector< Frame* >& list = state.getTreeState ().getChildren (rw::core::Ptr<const Frame>(this));
    return makeConstIteratorPair (_children, list);
}

Frame::iterator_pair Frame::getChildren (const State& state)
{
    const std::vector< Frame* >& dafs = state.getTreeState ().getChildren (rw::core::Ptr<Frame>(this));
    return makeIteratorPair (_children, dafs);
}

std::vector< Frame::Ptr > Frame::getChildrenList (const State& state)
{
    const std::vector< Frame* >& children = state.getTreeState ().getChildren (rw::core::Ptr<Frame>(this));

    std::vector< Frame::Ptr > allChildren;

    for (Frame*& child : _children) {
        allChildren.push_back (Frame::Ptr (child));
    }

    for (size_t i = 0; i < children.size (); ++i) {
        allChildren.push_back (Frame::Ptr (children[i]));
    }

    return allChildren;
}

Frame::const_iterator_pair Frame::getDafChildren (const State& state) const
{
    const std::vector< Frame* >& list = state.getTreeState ().getChildren (this);
    return makeConstIteratorPair (list);
}

Frame::iterator_pair Frame::getDafChildren (const State& state)
{
    const std::vector< Frame* >& list = state.getTreeState ().getChildren (this);
    return makeIteratorPair (list);
}

// Frame attachments.

void Frame::attachTo (rw::core::Ptr<Frame> parent, State& state)
{
    state.getTreeState ().attachFrame (this, parent);
}

bool Frame::isDAF ()
{
    return Kinematics::isDAF (this);
}

namespace rw{ namespace kinematics {
std::ostream& operator<< (std::ostream& out, const Frame& frame)
{
    return out << "Frame[" << frame.getName () << "]";
}
}}

rw::math::Transform3D<> Frame::wTf (const rw::kinematics::State& state) const
{
    return Kinematics::worldTframe (this, state);
}

rw::math::Transform3D<> Frame::fTf (const rw::core::Ptr< Frame> to, const rw::kinematics::State& state) const
{
    return Kinematics::frameTframe (this, to, state);
}

bool Frame::operator== (const Frame& rhs)
{
    if(StateData::operator==(rhs) && this->_parent == rhs._parent && this->_children.size() == rhs._children.size()){
        for(size_t i = 0; i < this->_children.size();i++){
            if(this->_children[i] != rhs._children[i]){
                return false;
            }
        }
        return true;
    }
    return false;
}