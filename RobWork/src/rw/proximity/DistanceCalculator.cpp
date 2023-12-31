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

#include "DistanceCalculator.hpp"

#include <RobWorkConfig.hpp>
#include <rw/common/ScopedTimer.hpp>
#include <rw/core/Exception.hpp>
#include <rw/kinematics/FKTable.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rw/models/Object.hpp>
#include <rw/models/WorkCell.hpp>

//#include <omp.h>
#include <float.h>
//#include <algorithm>
#include "ProximityStrategyData.hpp"

using namespace rw;
using namespace rw::math;
using namespace rw::common;
using namespace rw::core;
using namespace rw::kinematics;
using namespace rw::models;
using namespace rw::proximity;
using namespace rw::geometry;

namespace {
// Find the frame or crash.
Frame* lookupFrame (const std::map< std::string, Frame* >& frameMap, const std::string& frameName)
{
    const std::map< std::string, Frame* >::const_iterator pos = frameMap.find (frameName);
    if (pos == frameMap.end ())
        RW_THROW ("Frame " << StringUtil::quote (frameName) << " is not present in frame map.");

    return pos->second;
}

bool isInList (const FramePair& pair, const FramePairList& pairs)
{
    return std::find (pairs.cbegin (), pairs.cend (), pair) != pairs.cend ();
}
}    // namespace

DistanceCalculator::DistanceCalculator (WorkCell::Ptr workcell, DistanceStrategy::Ptr strategy) :
    _root (workcell->getWorldFrame ()), _strategy (strategy), _state (workcell->getDefaultState ())

{
    RW_ASSERT (strategy);
    RW_ASSERT (workcell);

    try {
        _setup = CollisionSetup::get (workcell);
    }
    catch (const Exception& exp) {
        RW_WARN (exp.what ());
    }
    initializeGeometry (workcell);
    initializeDistancePairs ();
}

DistanceCalculator::DistanceCalculator (rw::core::Ptr<Frame> root, WorkCell::Ptr workcell,
                                        DistanceStrategy::Ptr strategy, const State& initialState) :
    _root (root),
    _strategy (strategy), _state (initialState)
{
    RW_ASSERT (root);
    RW_ASSERT (workcell);

    try {
        _setup = CollisionSetup::get (workcell);
    }
    catch (const Exception& exp) {
        RW_WARN (exp.what ());
    }

    RW_ASSERT (strategy);

    initializeGeometry (workcell);
    initializeDistancePairs ();
}

DistanceCalculator::DistanceCalculator (const FramePairList& pairs,
                                        DistanceStrategy::Ptr strategy) :
    _cnt (0),
    _root (nullptr), _strategy (strategy), _distancePairs (pairs)
{
    RW_ASSERT (strategy);
    _thresholdStrategy = _strategy;
    _timer.resetAndPause ();
}

void DistanceCalculator::initializeGeometry (rw::core::Ptr< const WorkCell > wc)
{
    // Add all frames+geometries in workcell to distanceStrategy
    std::vector< Object::Ptr > objects = wc->getObjects ();
    State state                        = wc->getDefaultState ();
    for (Object::Ptr object : objects) {
        for (geometry::Geometry::Ptr geom : object->getGeometry (state)) {
            rw::core::Ptr<Frame> frame = geom->getFrame ();    // this is not const - should it be?
            RW_ASSERT (frame);
            _strategy->addModel (frame, geom);
            //_frameToModels[*frame] = _npstrategy->getModel(frame);
        }
    }
}

void DistanceCalculator::initializeDistancePairs ()
{
    _thresholdStrategy = _strategy;

    _distancePairs.clear ();

    // All frames reachable from the root.
    const std::vector< Frame* >& frames = Kinematics::findAllFrames (_root, _state);

    // All pairs of frames.
    FramePairList pairs;
    typedef std::vector< Frame* >::const_iterator I;
    for (I from = frames.cbegin (); from != frames.cend (); ++from) {
        if (_strategy->hasModel (*from)) {
            I to = from;
            for (++to; to != frames.cend (); ++to) {
                if (_strategy->hasModel (*to)) {
                    pairs.push_back (FramePair (*from, *to));
                }
            }
        }
    }

    // All pairs of frames to exclude.
    FramePairList exclude_pairs;
    const std::map< std::string, Frame* >& frameMap = Kinematics::buildFrameMap (_root, _state);
    const StringPairList& exclude                   = _setup.getExcludeList ();

    typedef StringPairList::const_iterator EI;
    for (EI p = exclude.cbegin (); p != exclude.cend (); ++p) {
        rw::core::Ptr<Frame> first  = lookupFrame (frameMap, p->first);
        rw::core::Ptr<Frame> second = lookupFrame (frameMap, p->second);
        exclude_pairs.push_back (FramePair (first.get(), second.get()));
        exclude_pairs.push_back (FramePair (second.get(), first.get()));
    }

    // Include in the final list only the pairs that are not present in the
    // exclude list.
    typedef FramePairList::const_iterator PLI;
    for (PLI p = pairs.cbegin (); p != pairs.cend (); ++p) {
        if (!isInList (*p, exclude_pairs))
            _distancePairs.push_back (*p);
    }

    _timer.resetAndPause ();
}

DistanceCalculator::~DistanceCalculator ()
{}

DistanceStrategy::Result
DistanceCalculator::distance (const State& state,
                              std::vector< DistanceStrategy::Result >* result) const
{
    _cnt++;
    ScopedTimer stimer (_timer);
    FKTable fk (state);

    if (result != NULL)
        result->clear ();

    ProximityStrategyData data;
    DistanceStrategy::Result distance;
    distance.distance = DBL_MAX;

    typedef FramePairList::const_iterator I;
    for (I p = _distancePairs.cbegin (); p != _distancePairs.cend (); ++p) {
        const rw::core::Ptr<Frame> a = p->first;
        const rw::core::Ptr<Frame> b = p->second;

        DistanceStrategy::Result* dist;
        if (distance.distance == DBL_MAX || _thresholdStrategy == NULL || result != NULL) {
            dist = &_strategy->distance (a, fk.get (*a), b, fk.get (*b), data);
        }
        else {
            dist = &_thresholdStrategy->distance (
                a, fk.get (*a), b, fk.get (*b), distance.distance, data);
        }

        dist->f1 = a;
        dist->f2 = b;

        if (dist->distance < distance.distance)
            distance = *dist;

        if (result != NULL)
            result->push_back (*dist);
    }

    return distance;
}

DistanceResult DistanceCalculator::distanceOMP (const State& state,
                                                std::vector< DistanceResult >* result) const
{
    _cnt++;
    ScopedTimer stimer (_timer);
    FKTable fk (state);

    if (result != NULL)
        result->clear ();

    ProximityStrategyData data;
    DistanceResult distance;
    distance.distance = DBL_MAX;
    // std::cout<<"Distance Pairs = "<<_distancePairs.size()<<std::endl;
    const int N = (int) _distancePairs.size ();
    int i;
    Transform3D<> ta, tb;

#ifdef RW_HAVE_OMP
#pragma omp parallel for shared(distance, result, fk) private(i, data, ta, tb) schedule(static, 1)
#endif
    for (i = 0; i < N; i++) {
        // for (I p = _distancePairs.begin(); p != _distancePairs.end(); ++p) {
        // const rw::core::Ptr<Frame> a = p->first;
        // const rw::core::Ptr<Frame> b = p->second;
        const rw::core::Ptr<Frame> a = _distancePairs[i].first;
        const rw::core::Ptr<Frame> b = _distancePairs[i].second;

        DistanceResult* dist;
#ifdef RW_HAVE_OMP
#pragma omp critical
#endif
        {
            ta = fk.get (*a);
            tb = fk.get (*b);
        }

        if (distance.distance == DBL_MAX || _thresholdStrategy == NULL || result != NULL) {
            dist = &_strategy->distance (a, ta, b, tb, data);
        }
        else {
            dist = &_thresholdStrategy->distance (a, ta, b, tb, distance.distance, data);
        }

        dist->f1 = a;
        dist->f2 = b;
#ifdef RW_HAVE_OMP
#pragma omp critical
#endif
        {
            if (dist->distance < distance.distance)
                distance = *dist;

            if (result != NULL)
                result->push_back (*dist);

        }    //#End #pragma omp critical
    }
    //	} //End #pragma omp parallel for schedule

    return distance;
}

DistanceStrategy::Result
DistanceCalculator::distance (const State& state, const rw::core::Ptr<Frame> frame,
                              std::vector< DistanceStrategy::Result >* result) const
{
    ScopedTimer stimer (_timer);
    FKTable fk (state);

    if (result != NULL)
        result->clear ();

    ProximityStrategyData data;
    DistanceStrategy::Result distance;
    distance.distance = DBL_MAX;
    typedef FramePairList::const_iterator I;
    for (I p = _distancePairs.cbegin (); p != _distancePairs.cend (); ++p) {
        const rw::core::Ptr<Frame> a = p->first;
        const rw::core::Ptr<Frame> b = p->second;

        if (a == frame || b == frame) {
            DistanceStrategy::Result* dist;
            if (distance.distance == DBL_MAX || result != NULL) {
                dist = &_strategy->distance (a, fk.get (*a), b, fk.get (*b), data);
            }
            else {
                dist = &_thresholdStrategy->distance (
                    a, fk.get (*a), b, fk.get (*b), distance.distance, data);
            }

            dist->f1 = a;
            dist->f2 = b;

            if (dist->distance < distance.distance)
                distance = *dist;

            if (result != NULL)
                result->push_back (*dist);
        }
    }

    return distance;
}

void DistanceCalculator::setDistanceStrategy (DistanceStrategy::Ptr strategy)
{
    RW_ASSERT (strategy);
    _strategy = strategy;
}

bool DistanceCalculator::addDistanceModel (const rw::core::Ptr<Frame> frame, const rw::geometry::Geometry& faces)
{
    bool res = _strategy->addModel (frame, faces);
    if (res)
        initializeDistancePairs ();
    return res;
}

void DistanceCalculator::clearCache ()
{
    // Clear frame pairs list
    _strategy->clear ();
    initializeDistancePairs ();
}
