#ifndef RWSIMULATOR_HPP_
#define RWSIMULATOR_HPP_

#include <rwsim/simulator/PhysicsEngine.hpp>
#include <rw/kinematics/FrameMap.hpp>
#include <rw/core/PropertyMap.hpp>
#include <rw/core/Ptr.hpp>
#include "RWBodyPool.hpp"

namespace rw { namespace kinematics { class State; } }

namespace rwsim {
namespace dynamics{
	class DynamicWorkCell;
	class RigidBody;
	class KinematicBody;
	class BodyController;
}

namespace simulator {

    class CNodePool;
    class ContactModelFactory;
    class ContactGraph;
    class ConstantForceManipulator;
    class BodyController;
    class BodyIntegrator;
    class RWDebugRender;
    class ConstraintSolver;

	class RWSimulator: public PhysicsEngine
	{
	public:

        RWSimulator();

		/**
		 * @brief constructor
		 */
		RWSimulator(rw::core::Ptr<rwsim::dynamics::DynamicWorkCell> dwc);

		/**
		 * @brief default destructor
		 */
		virtual ~RWSimulator(){
			exitPhysics();
		}

		//! @copydoc PhysicsEngine::load
		void load(rw::core::Ptr<rwsim::dynamics::DynamicWorkCell> dwc);

		//! @copydoc PhysicsEngine::setContactDetector
		bool setContactDetector(rw::core::Ptr<rwsim::contacts::ContactDetector> detector);

		/**
		 * @copydoc PhysicsEngine::initPhysics
		 */
		void initPhysics(rw::kinematics::State& state);

		/**
		 * @copydoc PhysicsEngine::step
		 */
		void step(double dt, rw::kinematics::State& state);

		/**
		 * @copydoc PhysicsEngine::resetScene
		 */
		void resetScene(rw::kinematics::State& state);

		/**
		 * @copydoc PhysicsEngine::exitPhysics
		 */
		void exitPhysics();

		/**
		 * @copydoc PhysicsEngine::getTime
		 */
		double getTime(){
			return _time;
		}

		void attach(dynamics::Body::Ptr b1, dynamics::Body::Ptr b2){}
		void detach(dynamics::Body::Ptr b1, dynamics::Body::Ptr b2){};
		/**
		 * @copydoc PhysicsEngine::createDebugRender
		 */
		drawable::SimulatorDebugRender::Ptr createDebugRender();

		virtual void setEnabled(dynamics::Body::Ptr body, bool enabled){}

		rw::core::PropertyMap& getPropertyMap()
		{ return _propertyMap; };

		void emitPropertyChanged(){}

		void addController(rw::core::Ptr<rwlibs::simulation::SimulatedController> controller){
			_controllers.push_back(controller);
		}

		void addBody(rwsim::dynamics::Body::Ptr body, rw::kinematics::State &state){

		}

		void addDevice(rw::core::Ptr<rwsim::dynamics::DynamicDevice> dev, rw::kinematics::State &state){

		}

		void addSensor(rwlibs::simulation::SimulatedSensor::Ptr sensor, rw::kinematics::State &state){
			_sensors.push_back(sensor);
		}

		void removeController(rw::core::Ptr<rwlibs::simulation::SimulatedController> controller){}

		void removeSensor(rwlibs::simulation::SimulatedSensor::Ptr sensor){};
		void setDynamicsEnabled(rwsim::dynamics::Body::Ptr body, bool enabled){}
		std::vector<rwlibs::simulation::SimulatedSensor::Ptr> getSensors(){ return _sensors;};

	private:
		rw::core::PropertyMap _propertyMap;

		/**
		 * @brief a step of dt if no penetrating collision occour, else a
		 * step until the collision occour.
		 * @param dt [in] the step to take.
		 * @param state [in] current state
		 * @return the actual time step dt that was taken
		 */
		double internalStep(double dt, rw::kinematics::State& state);

		void rollBack(rw::kinematics::State &state);

	private:
		rw::core::Ptr<rwsim::dynamics::DynamicWorkCell> _dwc;

		CNodePool *_pool;
		ContactModelFactory *_factory;
		ContactGraph *_cgraph;
		ConstraintSolver *_solver;

		RWBodyPool _bodyPool;
		std::vector<BodyController*> _manipulators;

		std::vector<RWBody*> _bodies;

		std::vector<rwsim::dynamics::RigidBody*> _rbodies;
		std::vector<rwsim::dynamics::KinematicBody*> _kbodies;
		std::vector<BodyIntegrator*> _integrators;

		double _time;

		rw::kinematics::FrameMap<RWBody*> _frameToBody;

		ConstantForceManipulator *_gravityManipulator;

		std::vector<rw::core::Ptr<rwlibs::simulation::SimulatedController> > _controllers;
		std::vector<rwlibs::simulation::SimulatedSensor::Ptr> _sensors;
	};

}
}

#endif /*RWSIMULATOR_HPP_*/
