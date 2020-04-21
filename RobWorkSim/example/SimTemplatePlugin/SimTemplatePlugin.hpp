#ifndef SimTemplatePlugin_HPP
#define SimTemplatePlugin_HPP

#include <rw/common/Timer.hpp>
#include <rws/RobWorkStudioPlugin.hpp>
#include "ui_SimTemplatePlugin.h"

#include <QObject>

namespace rwsim { namespace dynamics { class DynamicWorkCell; } }
namespace rwsim { namespace simulator { class DynamicSimulator; } }
namespace rwsim { namespace simulator { class ThreadSimulator; } }

class QTimer;

/**
 * @brief A plugin
 */
class SimTemplatePlugin: public rws::RobWorkStudioPlugin, private Ui::SimTemplatePlugin
{
    Q_OBJECT
    Q_INTERFACES( rws::RobWorkStudioPlugin )
    Q_PLUGIN_METADATA(IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "SimTemplatePlugin.json")
public:

    /**
     * @brief constructor
     */
    SimTemplatePlugin();

    //! destructor
    virtual ~SimTemplatePlugin();

    virtual void open(rw::models::WorkCell* workcell);

    virtual void close();

    virtual void initialize();

    /**
     * @brief we listen for events regarding opening and closing of dynamic
     * workcell
     */
    void genericEventListener(const std::string& event);

    void makeSimulator();
    void step(rwsim::simulator::ThreadSimulator* sim, const rw::kinematics::State& state);
    void startSimulation();

private slots:
    void btnPressed();
    void stateChangedListener(const rw::kinematics::State& state);

private:
    rw::models::WorkCell* _wc;
    rw::core::Ptr<rwsim::dynamics::DynamicWorkCell> _dwc;
    rw::core::Ptr<rwsim::simulator::ThreadSimulator> _tsim;
    rw::core::Ptr<rwsim::simulator::DynamicSimulator> _sim;

    QTimer *_timer;
    rw::common::Timer _wallTimer, _wallTotalTimer;
};

#endif
