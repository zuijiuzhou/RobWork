

#include "ScriptTypes.hpp"

#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rwslibs/rwstudioapp/RobWorkStudioApp.hpp>

#include <QApplication>

using namespace rws::swig;
using namespace rwlibs::swig;

rw::core::Ptr< rws::swig::RobWorkStudio > rwstudio_internal = NULL;

rws::swig::RobWorkStudio* rws::swig::getRobWorkStudio ()
{
    if (rwstudio_internal.isNull ()) {
        rwstudio_internal = getRobWorkStudioFromQt ();
        if (rwstudio_internal.isNull ()) {
            getRobWorkStudioInstance ();
        }
    }
    return rwstudio_internal.get ();
}
rws::swig::RobWorkStudio* rws::swig::getRobWorkStudioFromQt ()
{
    QWidget* rws_w    = NULL;
    QWidgetList all_w = QApplication::allWidgets ();
    for (QWidget* w : all_w) {
        if (w->objectName () == "RobWorkStudio_MainWindow") {
            rws_w = w;
        }
    }

    RobWorkStudio* rws_ = static_cast< RobWorkStudio* > (rws_w);
    return rws_;
}

void rws::swig::setRobWorkStudio (rws::swig::RobWorkStudio* rwstudio)
{
    rwstudio_internal = rwstudio;
}

const rw::kinematics::State& rws::swig::getState ()
{
    return getRobWorkStudio ()->getState ();
}
void rws::swig::setState (rw::kinematics::State& state)
{
    return getRobWorkStudio ()->postState (state);
}
rw::core::Ptr< Device > rws::swig::findDevice (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findDevice (name);
}
rw::core::Ptr< JointDevice > rws::swig::findJointDevice (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findDevice< JointDevice > (name);
}
rw::core::Ptr< SerialDevice > rws::swig::findSerialDevice (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findDevice< SerialDevice > (name);
}
rw::core::Ptr< TreeDevice > rws::swig::findTreeDevice (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findDevice< TreeDevice > (name);
}
rw::core::Ptr< ParallelDevice > rws::swig::findParallelDevice (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findDevice< ParallelDevice > (name);
}
rw::kinematics::Frame* rws::swig::findFrame (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findFrame (name);
}

rw::kinematics::MovableFrame* rws::swig::findMovableFrame (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findFrame< rw::kinematics::MovableFrame > (name);
}

rw::kinematics::FixedFrame* rws::swig::findFixedFrame (const std::string& name)
{
    return getRobWorkStudio ()->getWorkCell ()->findFrame< rw::kinematics::FixedFrame > (name);
}

void rws::swig::moveTo (rw::kinematics::MovableFrame* mframe,
                        rw::math::Transform3D< double > wTframe)
{
    rw::kinematics::State state = getState ();
    mframe->moveTo (wTframe, state);
    setState (state);
}

void rws::swig::moveTo (rw::kinematics::Frame* frame, rw::kinematics::MovableFrame* mframe,
                        rw::math::Transform3D< double > wTtcp)
{
    rw::kinematics::State state = getState ();
    rw::math::Transform3D< double > tcpTbase =
        rw::kinematics::Kinematics::frameTframe (frame, mframe, state);
    rw::math::Transform3D< double > wTbase_target = wTtcp * tcpTbase;
    mframe->moveTo (wTbase_target, state);
    setState (state);
}

void rws::swig::moveTo (const std::string& fname, const std::string& mname,
                        rw::math::Transform3D< double > wTframe)
{
    rw::kinematics::Frame* fframe        = findFrame (fname);
    rw::kinematics::MovableFrame* mframe = findMovableFrame (mname);
    moveTo (fframe, mframe, wTframe);
}

static rws::RobWorkStudioApp* robApp = NULL;

rw::core::Ptr< RobWorkStudio > rws::swig::getRobWorkStudioInstance (const std::string& args_tmp)
{
    // create a thread that start QApplication and
    std::cout << "Getting rws instance" << std::endl;
    if (robApp == NULL || !robApp->isRunning ()) {
        std::string args = args_tmp;
        std::cout << "adding extra arg: " << args << std::endl;

        robApp = new RobWorkStudioApp (args);
        std::cout << "Starting rws instance" << std::endl;
        robApp->start ();
        std::cout << "waiting for rws instance" << std::endl;
        while (robApp->getRobWorkStudio () == NULL) {
            if (!robApp->isRunning ())
                return NULL;
            rw::common::TimerUtil::sleepMs (100);
        }
        std::cout << "rws started" << std::endl;
        rwstudio_internal = robApp->getRobWorkStudio ();
    }
    else {
        std::cout << "rws already running" << std::endl;
    }
    return robApp->getRobWorkStudio ();
}

void rws::swig::closeRobWorkStudio ()
{
    robApp->close ();
}

bool rws::swig::isRunning ()
{
    if (robApp == NULL) {
        return false;
    }
    return robApp->isRunning ();
}

rw::math::Q rws::swig::getQ (rw::core::Ptr< rwlibs::swig::Device > dev)
{
    if (dev == NULL)
        RW_THROW ("Device is NULL!");
    return dev->getQ (getState ());
}
void rws::swig::setQ (rw::core::Ptr< rwlibs::swig::Device > dev, rw::math::Q q)
{
    if (dev == NULL)
        RW_THROW ("Device is NULL!");
    rw::kinematics::State state = getState ();
    dev->setQ (q, state);
    setState (state);
}

void rws::swig::setTransform (rw::kinematics::Frame* mframe,
                              rw::math::Transform3D< double > wTframe)
{
    if (rw::kinematics::FixedFrame* ff = dynamic_cast< rw::kinematics::FixedFrame* > (mframe)) {
        ff->setTransform (wTframe);
    }
    else if (rw::kinematics::MovableFrame* mf =
                 dynamic_cast< rw::kinematics::MovableFrame* > (mframe)) {
        rw::kinematics::State state = getState ();
        mf->setTransform (wTframe, state);
        setState (state);
    }
}

rw::math::Transform3D< double > rws::swig::wTf (rw::kinematics::Frame* frame)
{
    return rw::kinematics::Kinematics::worldTframe (frame, getState ());
}
rw::math::Transform3D< double > rws::swig::fTf (rw::kinematics::Frame* frame,
                                                rw::kinematics::Frame* to)
{
    return rw::kinematics::Kinematics::frameTframe (frame, to, getState ());
}
rw::math::Transform3D< double > rws::swig::wTf (const std::string& name)
{
    return rw::kinematics::Kinematics::worldTframe (findFrame (name), getState ());
}
rw::math::Transform3D< double > rws::swig::fTf (const std::string& frame, const std::string& to)
{
    return rw::kinematics::Kinematics::frameTframe (findFrame (frame), findFrame (to), getState ());
}

void rws::swig::addGeometry (const std::string& objName, rw::geometry::Geometry::Ptr geom)
{
    std::cout << "Not Implemented" << std::endl;
}

void rws::swig::addObject (const std::string& baseFrameName, rw::geometry::Geometry::Ptr geom)
{
    std::cout << "Not Implemented" << std::endl;
}

void rws::swig::removeObject (const std::string& objName)
{
    std::cout << "Not Implemented" << std::endl;
}
