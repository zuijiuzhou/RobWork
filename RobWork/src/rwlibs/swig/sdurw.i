%module sdurw

%{
#include <RobWorkConfig.hpp>
#include <rwlibs/swig/ScriptTypes.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/loaders/path/PathLoader.hpp>
#include <rw/loaders/dom/DOMPropertyMapLoader.hpp>
#include <rw/loaders/dom/DOMPropertyMapSaver.hpp>

using namespace rwlibs::swig;
using rw::math::Metric;
using namespace rw::math;
using rw::trajectory::Interpolator;
using rw::trajectory::Blend;
using rw::trajectory::Timed;
using rw::trajectory::Trajectory;
using rw::trajectory::InterpolatorTrajectory;
using rw::pathplanning::PathPlanner;
%}

%pragma(java) jniclassclassmodifiers="class"
#if defined (SWIGJAVA)
SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)
SWIG_JAVABODY_TYPEWRAPPER(public, public, public, SWIGTYPE)
#endif

#if defined(SWIGPYTHON)
%feature("doxygen:ignore:beginPythonOnly", range="end:endPythonOnly", contents="parse");
%feature("doxygen:ignore:beginJavaOnly", range="end:endJavaOnly");
#elif defined(SWIGJAVA)
%feature("doxygen:ignore:beginPythonOnly", range="end:endPythonOnly");
%feature("doxygen:ignore:beginJavaOnly", range="end:endJavaOnly", contents="parse");
#else
%feature("doxygen:ignore:beginPythonOnly", range="end:endPythonOnly");
%feature("doxygen:ignore:beginJavaOnly", range="end:endJavaOnly");
#endif

%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>

//%include <shared_ptr.i>

#if !defined(SWIGJAVA)
%include "carrays.i"
%array_class(double, doubleArray);
#else
%include "arrays_java.i";
#endif

#if defined(SWIGJAVA)
	%rename(multiply) operator*;
	%rename(divide) operator/;
	%rename(equals) operator==;
	%rename(negate) operator-() const;
	%rename(subtract) operator-;
	%rename(add) operator+;
#endif

#if (defined(SWIGPYTHON) || defined(SWIGLUA))
%feature("flatnested");
#endif

%include <stl.i>
%include <exception.i>


void writelog(const std::string& msg);

/********************************************
 * General utility functions
 ********************************************/

/* This is called for all functions to handle exceptions disable with %exception; 
 *  
 */
%exception {
    try {
        //printf("Entering function : $name\n"); // uncomment to get a print out of all function calls
        $action
    }catch(rw::core::Exception& e ){
        SWIG_exception(SWIG_RuntimeError,e.what());
    }catch(...){
        SWIG_exception(SWIG_RuntimeError,"unknown error");
    }
}

%inline %{
    void sleep(double t){
        ::rw::common::TimerUtil::sleepMs( (int) (t*1000) );
    }
    double time(){
        return ::rw::common::TimerUtil::currentTime( );
    }
    long long timeMs(){
        return ::rw::common::TimerUtil::currentTimeMs( );
    }
    void infoLog(const std::string& msg){
        ::rw::core::Log::infoLog() << msg << std::endl;
    }
    void debugLog(const std::string& msg){
        ::rw::core::Log::debugLog() << msg << std::endl;
    }
    void warnLog(const std::string& msg){
        ::rw::core::Log::warningLog() << msg << std::endl;
    }
    void errorLog(const std::string& msg){
        ::rw::core::Log::errorLog() << msg << std::endl;
    }
%}




/********************************************
 * Constants
 ********************************************/

%constant double Pi = rw::math::Pi;
%constant double Inch2Meter = rw::math::Inch2Meter;
%constant double Meter2Inch = rw::math::Meter2Inch;
%constant double Deg2Rad = rw::math::Deg2Rad;
%constant double Rad2Deg = rw::math::Rad2Deg;

/********************************************
 * STL vectors (primitive types)
 ********************************************/
#if (defined(SWIGLUA) || defined(SWIGPYTHON))
	%extend std::vector<std::string> { char *__str__() { return printCString(*$self); } }
#endif

namespace std {
	%template(VectorString) std::vector<string>;
	%template(VectorDouble) std::vector<double>;
	%template(VectorInt) std::vector<int>;
    %template(Vectorbool) std::vector<bool>;
    %template(pairDoubleDouble) std::pair<double,double>;
    %template(pairUIntUInt) std::pair<unsigned int, unsigned int>;
};

/********************************************
 * CORE
 ********************************************/
namespace rw { namespace core {

    class _n1
    {};
    class _n2
    {};
    class _n3
    {};
    class _n4
    {};
    class _n5
    {};
    class _n6
    {};
    class _n7
    {};
    class _n8
    {};
    template <class CallBackMethod, class T1 = _n1, class T2 = _n1, class T3 = _n1, class T4 = _n1>
    class Event
    {
    public:

    };

}}
/********************************************
 * COMMON
 ********************************************/

namespace rw { namespace core {

/**
  * @brief The Ptr type represents a smart pointer that can take ownership
  * of the underlying object.
  *
  * If the underlying object is owned by the smart pointer, it is destructed
  * when there is no more smart pointers pointing to the object.
  */
template<class T> class Ptr
{
public:
    //! @brief Empty smart pointer (Null).
    Ptr();

    /**
      * @brief Construct new smart pointer that takes ownership of the
      * underlying object.
      *
      * @param ptr The object to take ownership of.
      */
    Ptr(T* ptr);

    /**
      * @brief Construct smart pointer from other smart pointer.
      *
      * @param p the other (compatible) smart pointer.
      */
    template <class S>
    Ptr(const Ptr<S>& p);

    bool isShared();

    /**
      * @brief Check if smart pointer is null.
      *
      * @return true if smart pointer is null.
      */
    bool isNull();

    template<class A>
    bool operator==(const rw::core::Ptr<A>& p) const;

	%rename(dereference) get;

    T* get() const;

    T *operator->() const;
};

#if defined(SWIGPYTHON)
 %pythonprepend ownedPtr(T*) %{
  args[0].thisown = 0
 %}
#endif
/**
  * @brief Construct a smart pointer that takes ownership over a raw object \b ptr.
  *
  * @param ptr the object to take ownership of.
  */
template <class T>
Ptr<T> ownedPtr(T* ptr);

}}

%define OWNEDPTR(ownedPtr_type)
namespace rw { namespace core {
#if defined(SWIGJAVA)
 %typemap (in) ownedPtr_type* %{
  jclass objcls = jenv->GetObjectClass(jarg1_);
  const jfieldID memField = jenv->GetFieldID(objcls, "swigCMemOwn", "Z");
  jenv->SetBooleanField(jarg1_, memField, (jboolean)false);
  $1 = *(std::remove_const<ownedPtr_type>::type **)&jarg1;
 %}
#elif defined(SWIGLUA)
 %typemap (in,checkfn="SWIG_isptrtype") ownedPtr_type* %{
  if (!SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&$1,$descriptor,SWIG_POINTER_DISOWN))){
    SWIG_fail_ptr("$symname",$input,$descriptor);
  }
 %}
#endif
 %template (ownedPtr) ownedPtr<ownedPtr_type>;
#if (defined(SWIGLUA) || defined(SWIGJAVA))
 %clear ownedPtr_type*;
#endif
}}
%enddef

/** @addtogroup swig */
/* @{ */

//! @copydoc rw::core::PropertyMap
class PropertyMap
{
public: 
	//! @copydoc rw::core::PropertyMap::PropertyMap
	PropertyMap();
	//! @copydoc rw::core::PropertyMap::has
	bool has(const std::string& identifier) const;
    //! @copydoc rw::core::PropertyMap::size
    size_t size() const;
    //! @copydoc rw::core::PropertyMap::empty 
    bool empty() const;
    //! @copydoc rw::core::PropertyMap::erase
    bool erase(const std::string& identifier);
    
	%extend {
		
		bool getBool(const std::string& id){ return $self->get<bool>(id); }
		void setBool(const std::string& id, bool val){  $self->set<bool>(id,val); }
		void set(const std::string& id, bool val){  $self->set<bool>(id,val); }

		std::string getString(const std::string& id){ return $self->get<std::string>(id); }
		void setString(const std::string& id, std::string val){  $self->set<std::string>(id,val); }
		void set(const std::string& id, std::string val){  $self->set<std::string>(id,val); }
		
		std::vector<std::string>& getStringList(const std::string& id){ return $self->get<std::vector<std::string> >(id); }
		void setStringList(const std::string& id, std::vector<std::string> val){ $self->set<std::vector<std::string> >(id,val); }
		void set(const std::string& id, std::vector<std::string> val){ $self->set<std::vector<std::string> >(id,val); }
		
		rw::math::Q& getQ(const std::string& id){ return $self->get<rw::math::Q>(id); }
		void setQ(const std::string& id, rw::math::Q q){ $self->set<rw::math::Q>(id, q); }
		void set(const std::string& id, rw::math::Q q){ $self->set<rw::math::Q>(id, q); }

		rw::math::Pose6D<double>& getPose(const std::string& id){ return $self->get<rw::math::Pose6D<double> >(id); }
		void setPose6D(const std::string& id, rw::math::Pose6D<double> p){  $self->set<rw::math::Pose6D<double> >(id, p); }
		void set(const std::string& id, rw::math::Pose6D<double> p){  $self->set<rw::math::Pose6D<double> >(id, p); }
		
		rw::math::Vector3D<double>& getVector3(const std::string& id){ return $self->get<rw::math::Vector3D<double> >(id); }
		void setVector3(const std::string& id, rw::math::Vector3D<double> p){  $self->set<rw::math::Vector3D<double> >(id, p); }
		void set(const std::string& id, rw::math::Vector3D<double> p){  $self->set<rw::math::Vector3D<double> >(id, p); }

		rw::math::Transform3D<double> & getTransform3(const std::string& id){ return $self->get<rw::math::Transform3D<double> >(id); }
		void setTransform3(const std::string& id, rw::math::Transform3D<double>  p){  $self->set<rw::math::Transform3D<double> >(id, p); }
		void set(const std::string& id, rw::math::Transform3D<double>  p){  $self->set<rw::math::Transform3D<double> >(id, p); }

		PropertyMap& getMap(const std::string& id){ return $self->get<PropertyMap>(id); }
		void setMap(const std::string& id, PropertyMap p){  $self->set<PropertyMap>(id, p); }
		void set(const std::string& id, PropertyMap p){  $self->set<PropertyMap>(id, p); }

		void load(const std::string& filename){ *($self) = rw::loaders::DOMPropertyMapLoader::load(filename); }
		void save(const std::string& filename){ rw::loaders::DOMPropertyMapSaver::save( *($self), filename ); }
		
	}    
 
};
%template (PropertyMapPtr) rw::core::Ptr<PropertyMap>;
OWNEDPTR(PropertyMap)

/**
 * \brief Provides basic log functionality.
 *
 * The Log class owns a number of LogWriters in a static map, which can be accessed
 * using a string identifier. All logs are global.
 *
 * By default the Log class contains a Debug, Info, Warning and Error log. These can be accessed
 * statically as:
 * \code
 * Log::debugLog() <<  "This is an debug message";
 * Log::infoLog() << "This is an info message";
 * Log::warnLog() << "This is an error message";
 * Log::errorLog() << "This is an error message";
 * \endcode
 * or on the log instance
 * \code
 * Log &log = Log::log();
 * log.debug() <<  "This is an debug message";
 * log.info() << "This is an info message";
 * log.warn() << "This is an error message";
 * log.error() << "This is an error message";
 * \endcode
 * or using one one the RW_LOG, RW_LOGLINE or RW_LOG2 macros, e.g.
 * \code
 * RW_LOG_INFO("The value of x is "<<x);
 * RW_LOG_DEBUG("The value of x is "<<x);
 * RW_LOG_ERROR(Log::infoId(), "The value of x is "<<x);
 * \endcode
 *
 * You can control what logs are active both using a loglevel and by using a log mask.
 * The loglevel enables all logs with LogIndex lower or equal to the loglevel. As default
 * loglevel is LogIndex::info which means debug and all user logs are disabled. However,
 * logs can be individually enabled using log masks which will override loglevel setting.
 *
 * Notice that logmasks cannot disable logs that are below or equal to loglevel.
 *
 * change loglevel:
 * \code
 * Log::log().setLevel(Log::Debug);
 * \endcode
 *
 *
 */
class Log
{
public:
    //! @brief loglevel mask
	enum LogIndexMask {
		FatalMask=1, CriticalMask=2,
		ErrorMask=4, WarningMask=8,
		InfoMask=16, DebugMask=32,
		User1Mask=64, User2Mask=128,
		User3Mask=256, User4Mask=512,
		User5Mask=1024, User6Mask=2048,
		User7Mask=4096, User8Mask=8096,
		AllMask = 0xFFFF
	};



	/**
	 * @brief Indices for different logs. The loglevel will be Info as default. Everything below the
	 * loglevel is enabled.
	 */
	enum LogIndex {
		Fatal=0, Critical=1,
		Error=2, Warning=3,
		Info=4, Debug=5,
		User1=6, User2=7,
		User3=8, User4=9,
		User5=10, User6=11,
		User7=12, User8=13
	};

	/**
	 * @brief Convert a LogIndex to a mask.
	 *
	 * @param idx [in] the LogIndex.
	 * @return the mask enabling the given log level.
	 */
    static LogIndexMask toMask(LogIndex idx){
            LogIndexMask toMaskArr[] = {FatalMask, CriticalMask,
                                      ErrorMask, WarningMask,
                                                InfoMask, DebugMask,
                                                User1Mask, User2Mask,
                                                User3Mask, User4Mask,
                                                User5Mask, User6Mask,
                                                User7Mask, User8Mask,
                                                AllMask};
            return toMaskArr[idx];
        }

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the info loglevel
	 *
	 * @return info LogWriter
	 */
    static LogWriter& infoLog();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the warning loglevel
	 *
	 * @return warning LogWriter
	 */
    static LogWriter& warningLog();


	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the error loglevel
	 *
	 * @return error LogWriter
	 */
    static LogWriter& errorLog();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the debug loglevel
	 *
	 * @return debug LogWriter
	 */
    static LogWriter& debugLog();

	/**
	 * @brief returns the global log instance. Global in the sence
	 * of whatever is linked staticly together.
	 *
	 * @return a Log
	 */
    static rw::core::Ptr<Log> getInstance();

    /**
     * @brief convenience function of getInstance
     *
     * @return a Log
     */
    static Log& log();

    /**
     * @brief sets the instance of the log class
     *
     * @param log [in] the log that will be used through the static log methods.
     */
    static void setLog(rw::core::Ptr<Log> log);

    //************************* Here follows the member interface

    /**
     * @brief constructor
     */
    Log();

    /**
     * @brief Destructor
     */
    virtual ~Log();

    /**
     * @brief set the loglevel. Any log with LogIndex equal to or less than
     * loglevel will be enabled. Any log above will be disabled unless an
     * enabled mask is specified for that log
     *
     * @param loglevel [in] the level
     */
    void setLevel(LogIndex loglevel);


    /**
     * @brief gets the log writer associated to logindex \b id
     *
     * @param id [in] logindex
     * @return log writer
     */
    rw::core::Ptr<LogWriter> getWriter(LogIndex id);

    /**
     * @brief Associates a LogWriter with the LogIndex \b id.
     *
     * SetWriter can either be used to redefine an existing log or to create a new
     * custom output.
     *
     * Example:
     * \code
     * Log::SetWriter(Log::User1, new LogStreamWriter(std::cout));
     * RW_LOG(Log::User1, "Message send to User log 1");
     * \endcode
     *
     * @param id [in] the LogIndex that the logwriter is associated with.
     * @param writer [in] LogWriter object to use
     */
    void setWriter(LogIndex id, rw::core::Ptr<LogWriter> writer);

    /**
     * @brief Associates a LogWriter with the logs specified with \b mask.
     *
     * SetWriter can either be used to redefine an existing log or to create a new
     * custom output.
     *
     * Example:
     * \code
     * log.setWriterForMask(Log::InfoMask | Log::DebugMask, new LogStreamWriter(std::cout));
     * RW_LOG(Log::Info, "Message send to User log 1");
     * \endcode
     *
     * @param mask [in] the LogIndexMask that the logwriter is associated with.
     * @param writer [in] LogWriter object to use
     */
	void setWriterForMask(int mask, rw::core::Ptr<LogWriter> writer);

    %extend {
		/**
		 * @brief Returns the LogWriter that is associated with LogIndex \b id
		 *
		 * If the \b id is unknown an exception is thrown.
		 *
		 * @param id [in] loglevel
		 * @return Reference to LogWriter object
		 */
		LogWriter& getLogWriter(LogIndex id) {
			return $self->get(id);
		}
    };

    /**
     * @brief Writes \b message to the log
     *
     * If the \b id cannot be found an exception is thrown
     *
     * @param id [in] Log identifier
     * @param message [in] String message to write
     */
    void write(LogIndex id, const std::string& message);

    /**
     * @brief Writes \b message to the logwriter associated with LogIndex \b id
     *
     * If the \b id cannot be found an exception is thrown

     *
     * @param id [in] Log identifier
     * @param message [in] Message to write
     */
    void write(LogIndex id, const Message& message);

    /**
     * @brief Writes \b message followed by a '\\n' to the log
     *
     * If the \b id cannot be found an exception is thrown
     *
     * @param id [in] Log identifier
     * @param message [in] Message to write
     */
    void writeln(LogIndex id, const std::string& message);

    /**
     * @brief Calls flush on the specified log
     *
     * If the \b id cannot be found an exception is thrown
     *
     * @param id [in] loglevel
     */
    void flush(LogIndex id);


    /**
     * @brief Calls flush on all logs
     */
    void flushAll();


    /**
     * @brief Removes a log
     *
     * If the \b id cannot be found an exception is thrown
     *
     * @param id [in] Log identifier
     */
    void remove(LogIndex id);

	/**
	 * @brief Removes all log writers
	 */
	void removeAll();

	//! @brief Make indentation to make logs easier to read.
	void increaseTabLevel();

	//! @brief Decrease the indentation.
	void decreaseTabLevel();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the info loglevel
	 *
	 * @return info LogWriter
	 */
    LogWriter& info();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the warning loglevel
	 *
	 * @return info LogWriter
	 */
    LogWriter& warning();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the error loglevel
	 *
	 * @return info LogWriter
	 */
    LogWriter& error();

	/**
	 * @brief convenience function for getting the LogWriter
	 * that is associated with the debug loglevel
	 *
	 * @return info LogWriter
	 */
    LogWriter& debug();

	/**
	 * @brief Enable log(s) given by log mask.
	 *
	 * @param mask [in] the mask for the logs to enable.
	 */
	void setEnable(int mask);

   /**
     * @brief Checks if the given LogIndex is enabled. This can be used to
     * determine if a certain log level will be displayed or not.
     *
     * @param idx [in] the level
     *
     * @return true if enabled, false otherwise.
     */
    bool isEnabled(LogIndex idx);

	/**
	 * @brief Disable log(s) given by log mask.
	 *
	 * @param mask [in] the mask for the logs to disable.
	 */
	void setDisable(int mask);
};

%template (LogPtr) rw::core::Ptr<Log>;
OWNEDPTR(Log)

/**
 * @brief Write interface for Logs
 *
 * LogWriter provides an output strategy for a log.
 */
class LogWriter
{
public:
    /**
     * @brief Descructor
     */
    virtual ~LogWriter();

    /**
     * @brief Flush method
     */
    void flush();

	/**
	 * @brief Set the tab level
	 */
	void setTabLevel(int tabLevel);


    /**
     * @brief Writes \b str to the log
     *
     * @param str [in] message to write
     */
    void write(const std::string& str);

    /**
     * @brief Writes \b msg to the log
     *
     * Default behavior is to use write(const std::string&) for the standard
     * streaming representation of \b msg.
     *
     * @param msg [in] message to write
     */
    void write(const Message& msg);

    /**
     * @brief Writes \b str as a line
     *
     * By default writeln writes \b str followed by a '\\n'. However, logs
     * are free to implement a line change differently.
     */
    void writeln(const std::string& str);

    /**
     * @brief general stream operator
     */
    template< class T>
    LogWriter& operator<<( T t );

#if defined(SWIGPYTHON)
    /**
     * @brief specialized stream operator
     */
    LogWriter& operator<<(const std::string& str);

    /**
     * @brief Write Message to log.
     * @param msg [in] the message.
     * @return a reference to this LogWriter for chaining of stream operators.
     */
    LogWriter& operator<<(const Message& msg);


    /**
     * @brief specialized stream operator
     */
    LogWriter& operator<<(const char* str);

    /**
     * @brief Handle the std::endl and other stream functions.
     */
    LogWriter& operator<<(std::ostream& (*pf)(std::ostream&));
#endif

protected:
    LogWriter();

	virtual void doWrite(const std::string& message) = 0;
	virtual void doSetTabLevel(int tabLevel) = 0;
	virtual void doFlush() = 0;
};

%template (LogWriterPtr) rw::core::Ptr<LogWriter>;

/**
 * @brief Standard type for user messages of RobWork.
 *
 * Messages are used for exception, warnings, and other things that are
 * reported to the user.
 *
 * Message values should contain the source file name and line number so
 * that it is easy to look up the place in the code responsible for the
 * generation of the message.
 *
 * RW_THROW and RW_WARN of macros.hpp have been introduced for the throwing
 * of exceptions and emission of warnings.
 */
class Message
{
public:
    /**
     * @brief Constructor
     *
     * Messages of RobWork are all annotated by the originating file name,
     * the originating line number, and a message text for the user.
     *
     * Supplying all the file, line, and message parameters can be a little
     * painfull, so a utility for creating messages is available from the
     * file macros.hpp.
     *
     * @param file [in] The source file name.
     *
     * @param line [in] The source file line number.
     *
     * @param message [in] A message for a user.
     */
    Message(const std::string& file,
            int line,
            const std::string& message = "");

    /**
     * @brief The name of source file within which the message was
     * constructed.
     *
     * @return The exception file name.
     */
    const std::string& getFile() const;

    /**
     * @brief The line number for the file at where the message was
     * constructed.
     *
     * @return The exception line number.
     */
    int getLine() const;

    /**
     * @brief The message text meant for the user.
     *
     * @return The message text.
     */
    const std::string& getText() const;

    /**
     * @brief Returns a full description of the message containing file, line number and message.
     */
    std::string getFullText() const;

    /**
     * @brief general stream operator
     */
    template< class T>
    Message& operator<<( T t );
};

class ThreadPool { 
public:
    ThreadPool(int threads = -1);
    virtual ~ThreadPool();
    unsigned int getNumberOfThreads() const;
    void stop();
    bool isStopping();
	unsigned int getQueueSize();
	void waitForEmptyQueue();
};

%template (MessagePtr) rw::core::Ptr<Message>;

%template (ThreadPoolPtr) rw::core::Ptr<ThreadPool>;
OWNEDPTR(ThreadPool)

class ThreadTask {
public:
	typedef enum TaskState {
    	INITIALIZATION,
    	IN_QUEUE,
    	EXECUTING,
    	CHILDREN,
    	IDLE,
    	POSTWORK,
    	DONE
    } TaskState;

	ThreadTask(rw::core::Ptr<ThreadTask> parent);
	ThreadTask(rw::core::Ptr<ThreadPool> pool);
	virtual ~ThreadTask();
	bool setThreadPool(rw::core::Ptr<ThreadPool> pool);
	rw::core::Ptr<ThreadPool> getThreadPool();
	//virtual void run();
	//virtual void subTaskDone(ThreadTask* subtask);
	//virtual void idle();
	//virtual void done();
    bool execute();
    TaskState wait(ThreadTask::TaskState previous);
    void waitUntilDone();
    TaskState getState();
    bool addSubTask(rw::core::Ptr<ThreadTask> subtask);
    std::vector<rw::core::Ptr<ThreadTask> > getSubTasks();
    void setKeepAlive(bool keepAlive);
    bool keepAlive();
};

%template (ThreadTaskPtr) rw::core::Ptr<ThreadTask>;
%template (ThreadTaskPtrVector) std::vector<rw::core::Ptr<ThreadTask> >;
OWNEDPTR(ThreadTask)

class Plugin {
protected:
	 Plugin(const std::string& id, const std::string& name, const std::string& version);
	 
public:
	const std::string& getId();
    const std::string& getName();
    const std::string& getVersion();
};

%template (PluginPtr) rw::core::Ptr<Plugin>;
%template (PluginPtrVector) std::vector<rw::core::Ptr<Plugin> >;

struct ExtensionDescriptor {
	ExtensionDescriptor();
	ExtensionDescriptor(const std::string& id_, const std::string& point_);

    std::string id,name,point;
    rw::core::PropertyMap props;

    //rw::core::PropertyMap& getProperties();
    const rw::core::PropertyMap& getProperties() const;
};

class Extension {
public:
	Extension(ExtensionDescriptor desc, Plugin* plugin);
	
	const std::string& getId();
	const std::string& getName();
};

%template (ExtensionPtr) rw::core::Ptr<Extension>;
%template (ExtensionPtrVector) std::vector<rw::core::Ptr<Extension> >;

class ExtensionRegistry {
public:
	ExtensionRegistry();
	static rw::core::Ptr<ExtensionRegistry> getInstance();
	std::vector<rw::core::Ptr<Extension> > getExtensions(const std::string& ext_point_id) const;
	std::vector<rw::core::Ptr<Plugin> > getPlugins() const;
};

%template (ExtensionRegistryPtr) rw::core::Ptr<ExtensionRegistry>;

/**
 * @brief The timer class provides an easy to use platform independent timer
 *
 * In Windows the expected resolution is approx. 16ms.
 * In Linux the expected resolution is approx. 1ms
 */
class Timer
{
public:
    /**
     * @brief Constructor
     *
     * This implicitly starts the timer.
     */
    Timer();

    /**
     * @brief constructor - initialize the timer to a specified value. This does not start the timer.
     * @param timems [in] time in ms
     */
    Timer(long timems);

    /**
     * @brief constructor - initialize the timer to a specified value. This does not start the timer.
     * @param hh [in] hours
     * @param mm [in] minutes
     * @param ss [in] seconds
     * @param ms [in] milli seconds
     */
    Timer(int hh, int mm, int ss = 0, int ms = 0);

    /**
     * @brief Destructor
     */
    virtual ~Timer();

    /** 
     * @brief Returns true if the timer is paused
     * @return True is paused
     */
    bool isPaused();

    /**
     * @brief Reset the timer
     *
     * The timer is set back to zero and starts counting.
     *
     * It is OK to call reset() on a timer that has already been started:
     * The time will just be set back to zero again.
     */
    void reset();


    /**
     * @brief Resets and pauses the timer
     *
     * The timer is set to zero and paused
     *
     * It is OK to call reset() on a timer that has already been started:
     * The time will just be set back to zero again.
     */
    void resetAndPause();

    /**
     * @brief Resets and stats the timer
     *
     * Same as reset()
     *
     * The timer is set to zero and starts counting
     *
     * It is OK to call reset() on a timer that has already been started:
     * The time will just be set back to zero again.
     */
    void resetAndResume();

    /**
     * @brief Pause the timer
     *
     * The timer stops counting. As long as the timer has not been resumed
     * (see resume()) or restarted (see reset()) the timer value (see
     * getTime()) will stay the same.
     *
     * Is is OK to call pause() on a timer that has already been paused: The
     * timer will just stay paused and nothing is changed.
     */
    void pause();

    /**
     * @brief Resume the timer after a pause.
     *
     * The timer starts counting again.
     *
     * It is OK to call resume() on a timer that is already counting: The
     * timer keeps counting and nothing is done.
     */
    void resume();

    /**
     * @brief The time the timer has been running.
     *
     * The time passed while the timer has been paused are not included in
     * the running time. The timer is paused when pause() has been
     * called and counting is resumed when resume() has been called.
     *
     * It is perfectly OK and natural to call getTime() on a running timer,
     * i.e. a timer that has not been paused.
     *
     * A call of reset() resets the running time to zero.
     *
     * \return Time in seconds
     */
    double getTime() const;

    /**
     * @brief The time the timer has been running in hole seconds.
     *
     * see getTime
     *
     * \return Time in hole seconds
     */
    long getTimeSec() const;

    /**
     * @brief The time the timer has been running in mili seconds.
     *
     * see getTime
     *
     * \return Time in mili seconds
     */
    long getTimeMs() const;


    /**
     * @brief returns a string describing the time. The format of the time is described using \b format
     * @param format [in] the format is on the form:
     *  hh:mm:ss --> 05:06:08
     *  h:m:s --> 5:6:8
     * @return a formated time string
     */
    std::string toString(const std::string& format="hh:mm:ss");


    /**
     * @brief Returns system clock in hole seconds
     *
     * \warning The date/time at which this timer counts from is platform-specific, so
     * you should \b not use it for getting the calendar time. It's really only meant for
     * calculating wall time differences.
     */
    static long currentTimeSec();


    /**
     * @brief Returns system clock in milli-seconds
     *
     * \warning The date/time at which this timer counts from is platform-specific, so
     * you should \b not use it for getting the calendar time. It's really only meant for
     * calculating wall time differences.
     */
    static long currentTimeMs();


    /**
     * @brief Returns system clock in micro-seconds.
     *
     * \warning The date/time at which this timer counts from is platform-specific, so
     * you should \b not use it for getting the calendar time. It's really only meant for
     * calculating wall time differences.
     *
     * Notice: The timer cannot hold times longer than approx. 2100second.
     */
    static long currentTimeUs();

    /**
     * @brief Returns system clock in seconds
     *
     * \warning The date/time at which this timer counts from is platform-specific, so
     * you should \b not use it for getting the calendar time. It's really only meant for
     * calculating wall time differences.
     */
    static double currentTime();


    /**
     * @brief Sleeps for a period of time
     *
     * @param period [in] the time in miliseconds to sleep
     */
    static void sleepMs(int period);

    /**
     * @brief Sleeps for a period of time
     *
     * @param period [in] the time in microseconds to sleep
     */
    static void sleepUs(int period);
};

/********************************************
 * ROBWORK CLASS
 ********************************************/ 
 class RobWork {
 public:
	RobWork();
	
	static rw::core::Ptr<RobWork> getInstance();
	
	std::string getVersion() const;
	void initialize();
 };
 
 %template (RobWorkPtr) rw::core::Ptr<RobWork>;

/********************************************
 * GEOMETRY
 ********************************************/

class GeometryData {
public:
    typedef enum {PlainTriMesh,
                  IdxTriMesh,
                  SpherePrim, BoxPrim, OBBPrim, AABBPrim,
                  LinePrim, PointPrim, PyramidPrim, ConePrim,
                  TrianglePrim, CylinderPrim, PlanePrim, RayPrim,
                  UserType} GeometryType;

	/**
	 * @brief the type of this primitive
	 *
	 * @return the type of primitive.
	 */
    virtual GeometryType getType() const = 0;

	/**
	 * @brief gets a trimesh representation of this geometry data.
	 *
	 * The trimesh that is returned is by default a copy, which means
	 * ownership is transfered to the caller. 
	 * @param forceCopy Specifying forceCopy to false will enable copy by reference and 
     * ownership is not necesarilly transfered.
	 * This is more efficient, though pointer is only alive as long as this
	 * GeometryData is alive.
	 * @return TriMesh representation of this GeometryData
	 */
    virtual rw::core::Ptr<TriMesh> getTriMesh(bool forceCopy=true) = 0;

    /**
     * @brief test if this geometry data is convex
     * @return
     */
    virtual bool isConvex();

	/**
	 * @brief format GeometryType to string
	 * @param type [in] the type of geometry to convert to string.
	 * @return a string.
	 */
    static std::string toString(GeometryType type);
};

%template (GeometryDataPtr) rw::core::Ptr<GeometryData>;
OWNEDPTR(GeometryData);

class TriMesh: public GeometryData {
  public:
    /**
     * @brief gets the triangle at index idx.
     * @param idx [in] the index of the triangle.
     * @return the triangle at index idx
     */
    virtual rw::geometry::Triangle<double> getTriangle(size_t idx) const = 0;

    /**
     * @brief gets the triangle at index idx.
     * @param idx [in] the index of the triangle.
     * @param dst [out] where to store the triangle at index idx
     */
    virtual void getTriangle(size_t idx, rw::geometry::Triangle<double>& dst) const = 0;

    /**
     * @brief gets the triangle at index idx. Using Floating point presicion 
     * @param idx [in] the index of the triangle.
     * @param dst [out] where to store the triangle at index idx
     */
    virtual void getTriangle(size_t idx, rw::geometry::Triangle<float>& dst) const = 0;


    /**
     * @brief gets the number of triangles in the triangle array.
     */
    virtual size_t getSize() const = 0;

    /**
     * @brief gets the number of triangles in the triangle array.
     */
    virtual size_t size() const = 0;

    /**
     * @brief make a clone of this triangle mesh
     * @return clone of this trimesh
     */
    virtual rw::core::Ptr<TriMesh> clone() const = 0;

    /**
     * @brief Scale all vertices in the mesh.
     */
    virtual void scale(double scale) = 0;

    //! @copydoc GeometryData::getTriMesh
    rw::core::Ptr<TriMesh> getTriMesh(bool forceCopy=true);

    //! @copydoc GeometryData::isConvex
    virtual bool isConvex() { return _isConvex; }

    void setConvexEnabled(bool isConvex){ _isConvex = isConvex; }
    
    /**
     * @brief calculate a volume of this triangle mesh
     */
    double getVolume() const;
};

%template (TriMeshPtr) rw::core::Ptr<TriMesh>;

class Primitive: public GeometryData {
public:
    rw::core::Ptr<TriMesh> getTriMesh(bool forceCopy=true);
    virtual rw::core::Ptr<TriMesh> createMesh(int resolution) const = 0;
    virtual rw::math::Q getParameters() const = 0;
};

class Sphere: public Primitive {
public:
    //! constructor
    Sphere(const rw::math::Q& initQ);
    Sphere(double radi):_radius(radi);
    double getRadius();
    rw::core::Ptr<TriMesh> createMesh(int resolution) const;
    rw::math::Q getParameters() const;
    GeometryData::GeometryType getType() const;
};

class Box: public Primitive {
public:
    Box();
    Box(double x, double y, double z);
    Box(const rw::math::Q& initQ);
    rw::core::Ptr<TriMesh> createMesh(int resolution) const;
    rw::math::Q getParameters() const;
    GeometryType getType() const;
};

class Cone: public Primitive {
public:
    Cone(const rw::math::Q& initQ);
    Cone(double height, double radiusTop, double radiusBot);
    double getHeight();
    double getTopRadius();
    double getBottomRadius();
    rw::core::Ptr<TriMesh> createMesh(int resolution) const;
    rw::math::Q getParameters() const;
    GeometryType getType() const;
};

class Plane: public Primitive {
public:
    Plane(const rw::math::Q& q);
    Plane(const rw::math::Vector3D<double>& n, double d);
    Plane(const rw::math::Vector3D<double>& p1,
          const rw::math::Vector3D<double>& p2,
          const rw::math::Vector3D<double>& p3);

    rw::math::Vector3D<double>& normal();
    //const rw::math::Vector3D<double>& normal() const;
#if defined(SWIGJAVA)
	double d() const;
#else
    double& d();
#endif
    double distance(const rw::math::Vector3D<double>& point);
    double refit( std::vector<rw::math::Vector3D<double> >& data );
    rw::core::Ptr<TriMesh> createMesh(int resolution) const ;
    rw::math::Q getParameters() const;
    GeometryType getType() const;
};

/**
 * @brief Cylinder primitive.
 */
class Cylinder: public Primitive {
public:
    //! @brief Default constructor with no parameters.
	Cylinder();

	/**
	  * @brief Cylinder with parameters specified.
	  *
	  * @param radius the radius.
	  * @param height the height.
	  */

	Cylinder(float radius, float height);
	virtual ~Cylinder();
	double getRadius() const;
	double getHeight() const;
	
	/**
	  * @brief Create a mesh representation of the cylinder.
	  *
	  * @param resolution the resolution.
	  * @return the TriMesh.
	  */
	rw::core::Ptr<TriMesh> createMesh(int resolution) const;
	rw::math::Q getParameters() const;
	GeometryType getType() const;
};

class ConvexHull3D {
public:
    virtual void rebuild(const std::vector<rw::math::Vector3D<double> >& vertices) = 0;
    virtual bool isInside(const rw::math::Vector3D<double>& vertex) = 0;
    virtual double getMinDistInside(const rw::math::Vector3D<double>& vertex) = 0;
    virtual double getMinDistOutside(const rw::math::Vector3D<double>& vertex) = 0;
    virtual rw::core::Ptr<PlainTriMeshN1> toTriMesh() = 0;
};


class Geometry {
  public:
    /**
     * @brief constructor - autogenerated id from geometry type.
     * @param data
     * @param scale
     */
    Geometry(rw::core::Ptr<GeometryData> data, double scale=1.0);


    /**
     * @brief constructor giving a specified id.
     * @param data [in] pointer to geometry data
     * @param name [in] Unique name to be assigned for the geometry
     * @param scale [in] scaling factor
     */
    Geometry(rw::core::Ptr<GeometryData> data, const std::string& name, double scale=1.00);

    /**
     * @brief constructor - autogenerated id from geometry type.
     * @param data [in] pointer to geometry data
     * @param t3d [in] transform
     * @param scale [in] scaling factor
     */
    Geometry(rw::core::Ptr<GeometryData> data, const rw::math::Transform3D<double> & t3d,
             double scale=1.0);

    /**
     * @brief gets the scaling factor applied when using this geometry
     * @return the scale as double
     */
    double getScale() const;

    /**
     * @brief set the scaling factor that should be applied to
     * this geometry when used.
     * @param scale [in] scale factor
     */
    void setScale(double scale);

    /**
     * @brief set transformation
     * @param t2d [in] the new transform
     */
    void setTransform(const rw::math::Transform3D<double> & t3d);

    /**
     * @brief get transformation
     * @return the Current transform
     */
    const rw::math::Transform3D<double> & getTransform() const;

    /**
     * @brief get geometry data
     * @return the geometry data stored
     */
    rw::core::Ptr<GeometryData> getGeometryData();
#if !defined(SWIGJAVA)
    /**
     * @brief get geometry data
     * @return the geometry data stored
     */
    const rw::core::Ptr<GeometryData> getGeometryData() const;
#endif
    /**
     * @brief set transformation
     * @param data [in] the new geometry data
     */
    void setGeometryData(rw::core::Ptr<GeometryData> data);
    /**
     * @brief get name of this geometry
     * @return name as string
     */
    const std::string& getName() const;
    /**
     * @brief get file path of this geometry
     * @return the file path as string
     */
    const std::string& getFilePath() const;

    /**
     * @brief get identifier of this geometry
     * @return the id of the geometry
     */
    const std::string& getId() const;

    /**
     * @brief set name of this geometry
     * @param name [in] the new name of the geometry
     */
    void setName(const std::string& name);

    /**
     * @brief set file path this geometry
     * @param name [in] path to a geometry file
     */
    void setFilePath(const std::string& name);

    /**
     * @brief set identifier of this geometry
     * @param id [in] new id
     */
    void setId(const std::string& id);

    /**
     * @brief set the color of the geometry
     * @param red [in] the amount of red color 0-255
     * @param green [in] the amount of green color 0-255
     * @param blue [in] the amount of red color 0-255
     */
    void setColor(unsigned char red, unsigned char green, unsigned char blue);

    /**
     * @brief Get the reference frame.
     * @return the reference frame.
     */
    Frame* getFrame();

#if !define(SWIGJAVA)
    /**
     * @brief Get the reference frame.
     * @return the reference frame.
     */
    const Frame* getFrame() const;
#endif
    /**
     * @brief Set the draw mask.
     * @param mask [in] the draw mask.
     */
    void setMask(int mask);

    /**
     * @brief Get the draw mask.
     * @return the draw mask.
     */
    int getMask();

    /**
     * @brief util function for creating a Sphere geometry
     */
    static rw::core::Ptr<Geometry> makeSphere(double radi);

    /**
     * @brief util function for creating a Box geometry
     */
    static rw::core::Ptr<Geometry> makeBox(double x, double y, double z);

    /**
     * @brief util function for creating a Cone geometry
     */
    static rw::core::Ptr<Geometry> makeCone(double height, double radiusTop, double radiusBot);

    /**
     * @brief util function for creating a Cylinder geometry
     */
    static rw::core::Ptr<Geometry> makeCylinder(float radius, float height);

    /**
     * @brief Construct a grid.
     * @param dim_x [in] number of cells in first direction.
     * @param dim_y [in] number of cells in second direction.
     * @param size_x [in] size of one cell.
     * @param size_y [in] size of one cell.
     * @param xdir [in] the direction of the first dimension.
     * @param ydir [in] the direction of the second dimension.
     * @return a new grid geometry.
     */
    static rw::core::Ptr<Geometry> makeGrid(int dim_x, int dim_y,double size_x=1.0, double size_y=1.0,
                                        const rw::math::Vector3D<double>& xdir = rw::math::Vector3D<double>::x(),
                                        const rw::math::Vector3D<double>& ydir = rw::math::Vector3D<double>::y());

    /**
     * @brief get the color stored for the object
     * @param color [out] the array to store the color in
     */
    void getColor(float color[3]);
};
%template (GeometryPtr) rw::core::Ptr<Geometry>;
%template (GeometryPtrVector) std::vector<rw::core::Ptr<Geometry> >;
OWNEDPTR(Geometry);

class STLFile {
public:
    static void save(const TriMesh& mesh, const std::string& filename);
    static rw::core::Ptr<PlainTriMeshN1f> load(const std::string& filename);
};

class PlainTriMeshN1
{
};

%template (PlainTriMeshN1Ptr) rw::core::Ptr<PlainTriMeshN1>;

class PlainTriMeshN1f
{
};

%template (PlainTriMeshN1fPtr) rw::core::Ptr<PlainTriMeshN1f>;

%nodefaultctor Triangle;

namespace rw { namespace geometry{
    template <class T=double>
    class Triangle {
     public:
        //! @brief default constructor
	    Triangle(){};

	    /**
	     * @brief constructor
	     * @param p1 [in] vertice 1
	     * @param p2 [in] vertice 2
	     * @param p3 [in] vertice 3
	     */
	    Triangle(const rw::math::Vector3D<T>& p1,
                   const rw::math::Vector3D<T>& p2,
                   const rw::math::Vector3D<T>& p3);

	    /**
	     * @brief copy constructor
	     *
	     * @param f [in] - The face that is to be copied.
	     */
	    Triangle(const Triangle<T>& f);

	    /**
	     * @brief destructor
	     */
	    virtual ~Triangle(){};

	    /**
	     * @brief get vertex at index i
	     */
		rw::math::Vector3D<T>& getVertex(size_t i);

        /*
        //TODO(kalor) implement operators
        const rw::math::Vector3D<T>& operator[](size_t i) const;
        rw::math::Vector3D<T>& operator[](size_t i);
        */
       
		/**
		 * @brief calculates the face normal of this triangle. It is assumed
		 * that the triangle vertices are arranged counter clock wise.
		 */
		rw::math::Vector3D<T> calcFaceNormal() const ;

		/**
		 * @brief tests wheather the point x is inside the triangle
		 */
		bool isInside(const rw::math::Vector3D<T>& x);

		/**
		 * @brief calculate the area of the triangle
		 * @return area in m^2
		 */
        double calcArea() const;

        /**
         * @brief apply a transformation to this triangle
         * @param t3d [in] transform that is to be applied
         */
        void applyTransform(const rw::math::Transform3D<T>& t3d);

        /**
         * @brief Returns Triangle transformed by t3d.
         */
        Triangle<T> transform(const rw::math::Transform3D<T>& t3d) const;

#if !defined(SWIGJAVA)
        inline const Triangle<T>& getTriangle() const;
#endif
        inline Triangle<T>& getTriangle();

        //TODO(kalor) add print function
    };
}}
%template(TriangleD) rw::geometry::Triangle<double>;
%template(TriangleF) rw::geometry::Triangle<float>;


namespace rw { namespace geometry {
    /**
     * @brief indexed triangle class that has 3 indices that points to 3
     * vertices in an array typically used with the IndexedTriMesh  class.
     * the indice type (size) is templated.
     */
    template< class T = uint16_t > class IndexedTriangle
    {
      public:
        //! @brief default constructor
        IndexedTriangle (){};

        /**
         * @brief constructor
         * @param p1 [in] indice to vertice 1
         * @param p2 [in] indice to vertice 2
         * @param p3 [in] indice to vertice 3
         */
        IndexedTriangle (T p1, T p2, T p3);

        /**
         * @brief copy constructor
         *
         * @param f [in] - The face that is to be copied.
         */
        IndexedTriangle (const IndexedTriangle& f);

        /**
         * @brief returns the index of vertex i of the triangle
         */
        T& getVertexIdx (std::size_t i);

        #if !defined(SWIGJAVA)
            /**
             * @brief returns the index of vertex i of the triangle
             */
            const T& getVertexIdx (std::size_t i) const;
        #endif
        //TODO(kalor) implement template specilization

        /*
         * @brief tests wheather the point x is inside the triangle
         */
        /*template< class R >
        bool isInside (const rw::math::Vector3D< R >& x,
                       const std::vector< rw::math::Vector3D< R > >& verts);*/


        //TODO(kalor) add indexing (below functions)
        /*
         * @brief get vertex at index i
         */
        //T& operator[] (size_t i) { return getVertexIdx (i); }

        /*
         * @brief get vertex at index i
         */
        //const T& operator[] (size_t i) const { return getVertexIdx (i); }
    };
}}
%template(IndexedTriangleU16) rw::geometry::IndexedTriangle<>;
%template(IndexedTriangleU16Vector) std::vector<rw::geometry::IndexedTriangle<>>;


/**
 * @brief A simple point cloud data structure. Points may be ordered or not. An ordered set is
 * kept as a single array in row major order and with a width and a height. An unordered array
 * must have height==1 and width equal to the number of points.
 */
class PointCloud: public GeometryData {
public:
    /**
     * @brief constructor
     */
    PointCloud();

    /**
     * @brief constructor
     *
     * @param w [in]
     * @param h [in]
     */
    PointCloud(int w, int h);

	/**
	 * @brief destructor
	 */
	virtual ~PointCloud();

	//! @copydoc GeometryData::getType
	 GeometryType getType() const;

	/**
	 * @brief gets the number of points in the point cloud.
	 *
	 * @return the number of points.
	 */
	virtual size_t size() const;

	bool isOrdered();

    /**
     * @brief returns a char pointer to the image data
     *
     * @return const char pointer to the image data
     */
    const std::vector<rw::math::Vector3D<float> >& getData() const;

    /**
     * @brief width of the point cloud data. If the data is unordered then this
     * will be equal to the number of points.
     *
     * @return width of data points
     */
    int getWidth() const;

    int getHeight() const;

    /**
     * @brief set width of point cloud. Data elements are accessed as [x+y*width].
     *
     * If the current data array cannot contain the elements then it will be resized to
     * be able to it.
     *
     * @param w [in] new width
     * @param h [in] new height
     */
    void resize(int w, int h);

	//! @copydoc getTriMesh
	rw::core::Ptr<TriMesh> getTriMesh(bool forceCopy=true);

	const rw::math::Transform3D<float>& getDataTransform() const;

	/**
	 * @brief load point cloud from PCD file
	 *
	 * @param filename [in] name of PCD file
	 * @return a point cloud
	 */
	static rw::core::Ptr<PointCloud> loadPCD( const std::string& filename );

	/**
	 * @brief save point cloud in PCD file format (PCL library format)
	 *
	 * @param cloud [in] the point cloud to save
	 * @param filename [in] the name of the file to save to
	 * @param t3d [in] the transformation of the point cloud
	 */
    static void savePCD(const PointCloud& cloud,
                        const std::string& filename ,
                        const rw::math::Transform3D<float>& t3d =
                        rw::math::Transform3D<float>::identity());
};

%template (PointCloudPtr) rw::core::Ptr<PointCloud>;


/********************************************
 * GRAPHICS
 ********************************************/

%template (WorkCellScenePtr) rw::core::Ptr<WorkCellScene>;
%template (DrawableNodePtr) rw::core::Ptr<DrawableNode>;
%template (DrawableNodePtrVector) std::vector<rw::core::Ptr<DrawableNode> >;

OWNEDPTR(WorkCellScene);

%constant int DNodePhysical = DrawableNode::Physical;
%constant int DNodeVirtual = DrawableNode::Virtual;
%constant int DNodeDrawableObject = DrawableNode::DrawableObject;
%constant int DNodeCollisionObject = DrawableNode::CollisionObject;
%nodefaultctor DrawableNode;
%nodefaultctor WorkCellScene;

struct RenderInfo {
    /**
     * @brief Construct new rendering information.
     * @param mask [in] (optional) the draw type mask. Default is DrawableObject.
     */
    RenderInfo(unsigned int mask=4);
    //! @brief The DrawableTypeMask.
    unsigned int _mask;
    //! @brief The DrawType.
    //TODO(kalor) add DrawType _drawType;
    //! @brief Pointer to the state.
    State *_state;
    //! @brief Render transparently.
    bool _renderTransparent;
    //! @brief Render as a solid.
    bool _renderSolid;
    //! @brief Disabling rendering of normals.
    bool _disableNormalRender;
    //! @brief Rendering camera
    rw::core::Ptr<SceneCamera> _cam;
    //! @brief Transform  World to model
    rw::math::Transform3D<double> _wTm;
};

class DrawableNode {
public:

    enum DrawType {
        //! Render in solid
        SOLID,
        //! Render in wireframe
        WIRE,
        //! Render both solid and wireframe
        OUTLINE
    };

    virtual void setHighlighted(bool b) = 0;

    virtual bool isHighlighted() const = 0;

    virtual void setDrawType(DrawType drawType) = 0;

    virtual void setTransparency(float alpha) = 0;

    virtual float getTransparency() = 0;

    bool isTransparent();

    virtual void setScale(float scale) = 0;

    virtual float getScale() const = 0;

    virtual void setVisible(bool enable) = 0;

    virtual bool isVisible() = 0;

    virtual const rw::math::Transform3D<double> & getTransform() const  = 0;

    virtual void setTransform(const rw::math::Transform3D<double> & t3d) = 0;

    virtual void setMask(unsigned int mask) = 0;
    virtual unsigned int getMask() const = 0;
};

class Model3D {
public:
    Model3D(const std::string& name);
    virtual ~Model3D();
    //struct Material;
    //struct MaterialFaces;
    //struct MaterialPolys;
    //struct Object3D;
    //typedef enum{
    //    AVERAGED_NORMALS //! vertex normal is determine as an avarage of all adjacent face normals
    //    ,WEIGHTED_NORMALS //! vertex normal is determined as AVARAGED_NORMALS, but with the face normals scaled by the face area
    //    } SmoothMethod;
    //void optimize(double smooth_angle, SmoothMethod method=WEIGHTED_NORMALS);
    //int addObject(Object3D::Ptr obj);
    //void addGeometry(const Material& mat, rw::core::Ptr<Geometry> geom);
    //void addTriMesh(const Material& mat, const rw::geometry::TriMesh& mesh);
    //int addMaterial(const Material& mat);
    //Material* getMaterial(const std::string& matid);
    bool hasMaterial(const std::string& matid);
    void removeObject(const std::string& name);
    //std::vector<Material>& getMaterials();
    //std::vector<Object3D::Ptr>& getObjects();
    const rw::math::Transform3D<double>& getTransform();
    void setTransform(const rw::math::Transform3D<double>& t3d);
    const std::string& getName();
    void setName(const std::string& name);
    int getMask();
    void setMask(int mask);
    rw::core::Ptr<GeometryData> toGeometryData();
    bool isDynamic() const;
    void setDynamic(bool dynamic);
};

%template (Model3DPtr) rw::core::Ptr<Model3D>;
%template (Model3DPtrVector) std::vector<rw::core::Ptr<Model3D> >;
OWNEDPTR(Model3D);

class Render {
public:
    /**
     * @brief draws the object.
     * @param info [in] state and rendering specific info
     * @param type [in] the drawtype which is being used
     * @param alpha [in] the alpha value to render with
     */
    virtual void draw(const DrawableNode::RenderInfo& info, DrawableNode::DrawType type, double alpha) const = 0;
};

%template (RenderPtr) rw::core::Ptr<Render>;

class WorkCellScene {
 public:

     rw::core::Ptr<WorkCell> getWorkCell();

     void setState(const State& state);

     //rw::graphics::GroupNode::Ptr getWorldNode();
     void updateSceneGraph(State& state);
     //void clearCache();

     void setVisible(bool visible, Frame* f);

     bool isVisible(Frame* f);

     void setHighlighted( bool highlighted, Frame* f);
     bool isHighlighted( Frame* f);
     void setFrameAxisVisible( bool visible, Frame* f);
     bool isFrameAxisVisible( Frame* f);
     //void setDrawType( DrawableNode::DrawType type, Frame* f);
     //DrawableNode::DrawType getDrawType( Frame* f );

     void setDrawMask( unsigned int mask, Frame* f);
     unsigned int getDrawMask( Frame* f );
     void setTransparency(double alpha, Frame* f);

     //DrawableGeometryNode::Ptr addLines( const std::string& name, const std::vector<rw::geometry::Line >& lines, Frame* frame, int dmask=DrawableNode::Physical);
     //DrawableGeometryNode::Ptr addGeometry(const std::string& name, rw::core::Ptr<Geometry> geom, Frame* frame, int dmask=DrawableNode::Physical);
     rw::core::Ptr<DrawableNode> addFrameAxis(const std::string& name, double size, Frame* frame, int dmask=DrawableNode::Virtual);
     //rw::core::Ptr<DrawableNode> addModel3D(const std::string& name, rw::core::Ptr<Model3D> model, Frame* frame, int dmask=DrawableNode::Physical);
     //rw::core::Ptr<DrawableNode> addImage(const std::string& name, const rw::sensor::Image& img, Frame* frame, int dmask=DrawableNode::Virtual);
     //rw::core::Ptr<DrawableNode> addScan(const std::string& name, const rw::sensor::Scan2D& scan, Frame* frame, int dmask=DrawableNode::Virtual);
     //rw::core::Ptr<DrawableNode> addScan(const std::string& name, const rw::sensor::Image25D& scan, Frame* frame, int dmask=DrawableNode::Virtual);
     rw::core::Ptr<DrawableNode> addRender(const std::string& name, rw::core::Ptr<Render> render, Frame* frame, int dmask=DrawableNode::Physical);

     rw::core::Ptr<DrawableNode> addDrawable(const std::string& filename, Frame* frame, int dmask);
     void addDrawable(rw::core::Ptr<DrawableNode> drawable, Frame*);

     //std::vector<rw::core::Ptr<DrawableNode> > getDrawables();
     //std::vector<rw::core::Ptr<DrawableNode> > getDrawables(Frame* f);

     //std::vector<rw::core::Ptr<DrawableNode> > getDrawablesRec(Frame* f, State& state);
     rw::core::Ptr<DrawableNode> findDrawable(const std::string& name);

     rw::core::Ptr<DrawableNode> findDrawable(const std::string& name, Frame* frame);

     std::vector<rw::core::Ptr<DrawableNode> > findDrawables(const std::string& name);

     bool removeDrawables(Frame* f);

     bool removeDrawables(const std::string& name);

     bool removeDrawable(rw::core::Ptr<DrawableNode> drawable);

     bool removeDrawable(rw::core::Ptr<DrawableNode> drawable, Frame* f);

     bool removeDrawable(const std::string& name);
     bool removeDrawable(const std::string& name, Frame* f);
     Frame* getFrame(rw::core::Ptr<DrawableNode>  d);

     //rw::graphics::GroupNode::Ptr getNode(Frame* frame);
 };

%nodefaultctor SceneViewer;
class SceneViewer
{
};
 
%template (SceneViewerPtr) rw::core::Ptr<SceneViewer>;

/********************************************
 * GRASPPLANNING
 ********************************************/

/********************************************
 * INVKIN
 ********************************************/
 
 %include <rwlibs/swig/rw_i/invkin.i>

/********************************************
 * KINEMATICS
 ********************************************/

%include <rwlibs/swig/rw_i/kinematics.i>

/********************************************
 * LOADERS
 ********************************************/

/**
 * @brief Extendible interface for loading of WorkCells from files.
 *
 * By default, the following formats are supported:
 *
 * - File extensions ".wu", ".wc", ".tag", ".dev" will be loaded using
 *   the TULLoader.
 * - Remaining file extensions will be loaded using the standard RobWork
 *   XML format (XMLRWLoader).
 *
 * The Factory defines an extension point "rw.loaders.WorkCellLoader"
 * that makes it possible to add loaders for other file formats than the
 * ones above. Extensions take precedence over the default loaders.
 *
 * The WorkCell loader is chosen based on a case-insensitive file extension
 * name. So "scene.wc.xml" will be loaded by the same loader as
 * "scene.WC.XML"
 *
 * WorkCells are supposed to be loaded using the WorkCellLoaderFactory.load function:
 * @beginPythonOnly
 * ::\n
 *     wc = WorkCellLoaderFactory.load("scene.wc.xml")
 *     if wc.isNull():
 *         raise Exception("WorkCell could not be loaded")
 * @endPythonOnly
 * @beginJavaOnly <pre> \code
 * WorkCellPtr wc = WorkCellLoaderFactory.load("scene.wc.xml");
 * if (wc.isNull())
 *     throw new Exception("WorkCell could not be loaded.");
 * \endcode </pre> @endJavaOnly
 * Alternatively a WorkCell can be loaded in the less convenient way:
 * @beginPythonOnly
 * ::\n
 *    loader = WorkCellLoaderFactory.getWorkCellLoader(".wc.xml");
 *    wc = loader.load("scene.wc.xml")
 *    if wc.isNull():
 *        raise Exception("WorkCell could not be loaded")
 * @endPythonOnly
 * @beginJavaOnly <pre> \code
 * WorkCellLoaderPtr loader = WorkCellLoaderFactory.getWorkCellLoader(".wc.xml");
 * WorkCellPtr wc = loader.loadWorkCell("scene.wc.xml");
 * if (wc.isNull())
 *     throw new Exception("WorkCell could not be loaded.");
 * \endcode </pre> @endJavaOnly
 */
class WorkCellLoader {
public:
	virtual ~WorkCellLoader();
    /**
     * @brief Load a WorkCell from a file.
     *
     * @param filename [in] path to workcell file.
     */
	virtual rw::core::Ptr<WorkCell> loadWorkCell(const std::string& filename) = 0;

protected:
	WorkCellLoader();
};

%template (WorkCellLoaderPtr) rw::core::Ptr<WorkCellLoader>;

/**
 * @brief A factory for WorkCellLoader. This factory also defines the
 * "rw.loaders.WorkCellLoader" extension point where new loaders can be
 * registered.
 */
class WorkCellLoaderFactory {
public:
	/**
	 * @brief Get loaders for a specific format.
	 *
	 * @param format [in] the extension (including initial dot).
	 * The extension name is case-insensitive.
	 * @return a suitable loader.
	 */
	static rw::core::Ptr<WorkCellLoader> getWorkCellLoader(const std::string& format);

    /**
     * @brief Loads/imports a WorkCell from a file.
     *
     * An exception is thrown if the file can't be loaded.
     * The RobWork XML format is supported by default, as well as
     * TUL WorkCell format.
     *
     * @param filename [in] name of the WorkCell file.
     */
	static rw::core::Ptr<WorkCell> load(const std::string& filename);
private:
	WorkCellLoaderFactory();
};

class ImageLoader {
public:
	virtual ~ImageLoader();
	virtual rw::core::Ptr<Image> loadImage(const std::string& filename) = 0;
	virtual std::vector<std::string> getImageFormats() = 0;
	virtual bool isImageSupported(const std::string& format);
};

%template (ImageLoaderPtr) rw::core::Ptr<ImageLoader>;

class ImageLoaderFactory {
public:
	ImageLoaderFactory();
	static rw::core::Ptr<ImageLoader> getImageLoader(const std::string& format);
	static bool hasImageLoader(const std::string& format);
	static std::vector<std::string> getSupportedFormats();
};

#if defined(RW_HAVE_XERCES)

class XMLTrajectoryLoader
{
public:
    XMLTrajectoryLoader(const std::string& filename, const std::string& schemaFileName = "");
    XMLTrajectoryLoader(std::istream& instream, const std::string& schemaFileName = "");

    enum Type { QType = 0, Vector3DType, Rotation3DType, Transform3DType};
    Type getType();
    rw::core::Ptr<Trajectory<rw::math::Q> > getQTrajectory();
    rw::core::Ptr<Trajectory<rw::math::Vector3D<double> > > getVector3DTrajectory();
    rw::core::Ptr<Trajectory<rw::math::Rotation3D<double> > > getRotation3DTrajectory();
    rw::core::Ptr<Trajectory<rw::math::Transform3D<double> > > getTransform3DTrajectory();
};

class XMLTrajectorySaver
{
public:
    static bool save(const Trajectory<rw::math::Q>& trajectory, const std::string& filename);
    static bool save(const Trajectory<rw::math::Vector3D<double> >& trajectory, const std::string& filename);
    static bool save(const Trajectory<rw::math::Rotation3D<double> >& trajectory, const std::string& filename);
    static bool save(const Trajectory<rw::math::Transform3D<double> >& trajectory, const std::string& filename);
    static bool write(const Trajectory<rw::math::Q>& trajectory, std::ostream& outstream);
    static bool write(const Trajectory<rw::math::Vector3D<double> >& trajectory, std::ostream& outstream);
    static bool write(const Trajectory<rw::math::Rotation3D<double> >& trajectory, std::ostream& outstream);
    static bool write(const Trajectory<rw::math::Transform3D<double> >& trajectory, std::ostream& outstream);
private:
    XMLTrajectorySaver();
};

#endif

/********************************************
 * MATH
 ********************************************/
%include <rwlibs/swig/rw_i/math.i>

// Utility function within rw::Math
rw::math::Rotation3D<double> getRandomRotation3D();
rw::math::Transform3D<double>  getRandomTransform3D(const double translationLength = 1);

namespace rw { namespace math {
    class Math
    {
    public:
        Math() = delete;
        ~Math() = delete;

        /**
         * @brief Quaternion to equivalent angle axis conversion.
         *
         * @param quat [in] the Quaternion object that is to be converted.
         *
         * @return a EAA object that represents the converted quaternion
         */
        template <class A>
        static rw::math::EAA<A> quaternionToEAA(const rw::math::Quaternion<A> &quat);

        /**
         * @brief Equivalent angle axis to quaternion conversion.
         *
         * @param eaa [in] the EAA object that is to be converted
         *
         * @return a Quaternion object that represents the converted EAA
         */
        template <class A>
        static rw::math::Quaternion<A> eaaToQuaternion(const rw::math::EAA<A> &eaa);

        static inline double clamp(double val, double min, double max);

        static rw::math::Q clampQ(const rw::math::Q& q,
                                  const rw::math::Q& min,
                                  const rw::math::Q& max);

        static rw::math::Q clampQ(const rw::math::Q& q,
                                  const std::pair<rw::math::Q, rw::math::Q>& bounds);

        static rw::math::Vector3D<double> clamp(const rw::math::Vector3D<double>& q,
                                          const rw::math::Vector3D<double>& min,
                                          const rw::math::Vector3D<double>& max);

        static double ran();

        static void seed(unsigned seed);

        static void seed();

        static double ran(double from, double to);

        static int ranI(int from, int to);

        static double ranNormalDist(double mean, double sigma);

        static rw::math::Q ranQ(const rw::math::Q& from, const rw::math::Q& to);

        static rw::math::Q ranQ(const std::pair<rw::math::Q,rw::math::Q>& bounds);

        static rw::math::Q ranDir(size_t dim, double length = 1);
        
        static rw::math::Q ranWeightedDir(size_t dim, const rw::math::Q& weights, double length = 1);

        static double round(double d);

        static rw::math::Q sqr(const rw::math::Q& q);

        static rw::math::Q sqrt(const rw::math::Q& q);

        static rw::math::Q abs(const rw::math::Q& v);

        static double min(const rw::math::Q& v);

        static double max(const rw::math::Q& v);

        static double sign(double s);

        static rw::math::Q sign(const rw::math::Q& q);

        static int ceilLog2(int n);
        
        static long long factorial(long long n);

        static bool isNaN(double d);
    };
}} // end namespaces

%template (quaternionToEAA) rw::math::Math::quaternionToEAA<double>;
%template (quaternionToEAA) rw::math::Math::quaternionToEAA<float>;
%template (eaaToQuaternion) rw::math::Math::eaaToQuaternion<double>;
%template (eaaToQuaternion) rw::math::Math::eaaToQuaternion<float>;


/********************************************
 * MODELS
 ********************************************/
 %include <rwlibs/swig/rw_i/models.i>


/********************************************
 * PATHPLANNING
 ********************************************/

%include <rwlibs/swig/rw_i/planning.i>

/********************************************
 * PLUGIN
 ********************************************/

/********************************************
 * PROXIMITY
 ********************************************/

%include <rwlibs/swig/rw_i/proximity.i>

/********************************************
 * SENSOR
 ********************************************/

%include <rwlibs/swig/rw_i/sensor.i>

/********************************************
 * TRAJECTORY
 ********************************************/

template <class T>
class Timed
{
public:
    Timed();
    Timed(double time, const T& value);

    double getTime() const;
    T& getValue();

    %extend {
        void setTime(double time){
            $self->rw::trajectory::Timed<T>::getTime() = time;
        }
    };
};

%template (TimedQ) Timed<rw::math::Q>;
%template (TimedState) Timed<State>;

namespace rw { namespace trajectory {
template <class T>
class Path: public std::vector<T>
{
  public:

    Path();
    Path(size_t cnt);
    Path(size_t cnt, const T& value);
    Path(const std::vector<T>& v);

#if (defined (SWIGJAVA) && SWIG_VERSION >= 0x040000)
    %extend {
        int size(){ return boost::numeric_cast<int>($self->std::vector<T >::size()); }
        T& elem(size_t idx){ return (*$self)[idx]; }
    };
#else
    %extend {
        size_t size(){ return $self->std::vector<T >::size(); }
        T& elem(size_t idx){ return (*$self)[idx]; }
    };
#endif
};
}}


%template (PathState) rw::trajectory::Path<State>;
%template (PathStatePtr) rw::core::Ptr<rw::trajectory::Path<State>>;
OWNEDPTR(rw::core::Ptr<rw::trajectory::Path<State>>)

%template (TimedQVector) std::vector<Timed<rw::math::Q> >;
%template (TimedStateVector) std::vector<Timed<State> >;
%template (TimedQVectorPtr) rw::core::Ptr<std::vector<Timed<rw::math::Q> > >;
%template (TimedStateVectorPtr) rw::core::Ptr<std::vector<Timed<State> > >;
OWNEDPTR(std::vector<Timed<rw::math::Q> > )
//OWNEDPTR(std::vector<Timed<State> > )

%template (PathSE3) rw::trajectory::Path<rw::math::Transform3D<double> >;
%template (PathSE3Ptr) rw::core::Ptr<rw::trajectory::Path<rw::math::Transform3D<double> > >;
%template (PathQ) rw::trajectory::Path<rw::math::Q>;
%template (PathQPtr) rw::core::Ptr<rw::trajectory::Path<rw::math::Q> >;
%template (PathTimedQ) rw::trajectory::Path<Timed<rw::math::Q> >;
%template (PathTimedQPtr) rw::core::Ptr<rw::trajectory::Path<Timed<rw::math::Q> > >;
%template (PathTimedState) rw::trajectory::Path<Timed<State> >;
%template (PathTimedStatePtr) rw::core::Ptr<rw::trajectory::Path<Timed<State> > >;
OWNEDPTR(rw::trajectory::Path<rw::math::Transform3D<double> > )
OWNEDPTR(rw::trajectory::Path<rw::math::Q> )
OWNEDPTR(rw::trajectory::Path<Timed<rw::math::Q> > )
OWNEDPTR(rw::trajectory::Path<Timed<State> > )

%extend rw::trajectory::Path<rw::math::Q> {
    rw::core::Ptr<rw::trajectory::Path<Timed<rw::math::Q> > > toTimedQPath(rw::math::Q speed){
        rw::trajectory::TimedQPath tpath =
                rw::trajectory::TimedUtil::makeTimedQPath(speed, *$self);
        return rw::core::ownedPtr( new rw::trajectory::TimedQPath(tpath) );
    }

    rw::core::Ptr<rw::trajectory::Path<Timed<rw::math::Q> > > toTimedQPath(rw::core::Ptr<Device> dev){
        rw::trajectory::TimedQPath tpath =
                rw::trajectory::TimedUtil::makeTimedQPath(*dev, *$self);
        return rw::core::ownedPtr( new rw::trajectory::TimedQPath(tpath) );
    }

    rw::core::Ptr<rw::trajectory::Path<Timed<State> > > toTimedStatePath(rw::core::Ptr<Device> dev,
                                                     const State& state){
        rw::trajectory::Path<Timed<State>> tpath =
                rw::trajectory::TimedUtil::makeTimedStatePath(*dev, *$self, state);
        return rw::core::ownedPtr( new rw::trajectory::Path<Timed<State>>(tpath) );
    }

};

%extend rw::trajectory::Path<Timed<State> > {
	
	static rw::core::Ptr<rw::trajectory::Path<Timed<State> > > load(const std::string& filename, rw::core::Ptr<WorkCell> wc){
		rw::core::Ptr<rw::trajectory::Path<Timed<State>>> spath = 
                    rw::core::ownedPtr(new rw::trajectory::Path<Timed<State>>);
                *spath = rw::loaders::PathLoader::loadTimedStatePath(*wc, filename);
		return rw::core::Ptr<rw::trajectory::Path<Timed<State>>>( spath );
	}
	
	void save(const std::string& filename, rw::core::Ptr<WorkCell> wc){		 		
		rw::loaders::PathLoader::storeTimedStatePath(*wc,*$self,filename); 
	}
	
	void append(rw::core::Ptr<rw::trajectory::Path<Timed<State> > > spath){
		double startTime = 0;
		if($self->size()>0)
			startTime = (*$self).back().getTime(); 
		
		for(size_t i = 0; i<spath->size(); i++){
			Timed<State> tstate = (*spath)[i]; 
			tstate.getTime() += startTime;
			(*$self).push_back( tstate );
		}
	}
	
};

%extend rw::trajectory::Path<State > {
	
	static rw::core::Ptr<rw::trajectory::Path<State> > load(const std::string& filename, rw::core::Ptr<WorkCell> wc){
            rw::core::Ptr<rw::trajectory::Path<State>> spath = rw::core::ownedPtr(new rw::trajectory::StatePath);
            *spath = rw::loaders::PathLoader::loadStatePath(*wc, filename);
		return spath;
	}
	
	void save(const std::string& filename, rw::core::Ptr<WorkCell> wc){		 		
		rw::loaders::PathLoader::storeStatePath(*wc,*$self,filename); 
	}
	
	void append(rw::core::Ptr<rw::trajectory::Path<State> > spath){		
		for(size_t i = 0; i<spath->size(); i++){
			(*$self).push_back( (*spath)[i] );
		}
	}
	
	
	rw::core::Ptr<rw::trajectory::Path<Timed<State> > > toTimedStatePath(double timeStep){
		rw::core::Ptr<rw::trajectory::Path<Timed<State>>> spath = 
			rw::core::ownedPtr( new rw::trajectory::Path<Timed<State>>() );	
		for(size_t i = 0; i < $self->size(); i++){
			Timed<State> tstate(timeStep*i, (*$self)[i]); 
			spath->push_back( tstate );
		}	
		return spath;
	}
	
};



template <class T>
class Blend
{
public:
    virtual T x(double t) const = 0;
    virtual T dx(double t) const = 0;
    virtual T ddx(double t) const = 0;
    virtual double tau1() const = 0;
    virtual double tau2() const = 0;
};

%template (BlendR1) Blend<double>;
%template (BlendR2) Blend<rw::math::Vector2D<double> >;
%template (BlendR3) Blend<rw::math::Vector3D<double> >;
%template (BlendSO3) Blend<rw::math::Rotation3D<double> >;
%template (BlendSE3) Blend<rw::math::Transform3D<double> >;
%template (BlendQ) Blend<rw::math::Q>;

%template (BlendR1Ptr) rw::core::Ptr<Blend<double> >;
%template (BlendR2Ptr) rw::core::Ptr<Blend<rw::math::Vector2D<double> > >;
%template (BlendR3Ptr) rw::core::Ptr<Blend<rw::math::Vector3D<double> > >;
%template (BlendSO3Ptr) rw::core::Ptr<Blend<rw::math::Rotation3D<double> > >;
%template (BlendSE3Ptr) rw::core::Ptr<Blend<rw::math::Transform3D<double> > >;
%template (BlendQPtr) rw::core::Ptr<Blend<rw::math::Q> >;

OWNEDPTR(Blend<double> )
OWNEDPTR(Blend<rw::math::Vector2D<double> > )
OWNEDPTR(Blend<rw::math::Vector3D<double> > )
OWNEDPTR(Blend<rw::math::Rotation3D<double> > )
OWNEDPTR(Blend<rw::math::Transform3D<double> > )
OWNEDPTR(Blend<rw::math::Q> )

template <class T>
class Interpolator
{
public:
    virtual T x(double t) const = 0;
    virtual T dx(double t) const = 0;
    virtual T ddx(double t) const = 0;
    virtual double duration() const = 0;
};

%template (InterpolatorR1) Interpolator<double>;
%template (InterpolatorR2) Interpolator<rw::math::Vector2D<double> >;
%template (InterpolatorR3) Interpolator<rw::math::Vector3D<double> >;
%template (InterpolatorSO3) Interpolator<rw::math::Rotation3D<double> >;
%template (InterpolatorSE3) Interpolator<rw::math::Transform3D<double> >;
%template (InterpolatorQ) Interpolator<rw::math::Q>;

%template (InterpolatorR1Ptr) rw::core::Ptr<Interpolator<double> >;
%template (InterpolatorR2Ptr) rw::core::Ptr<Interpolator<rw::math::Vector2D<double> > >;
%template (InterpolatorR3Ptr) rw::core::Ptr<Interpolator<rw::math::Vector3D<double> > >;
%template (InterpolatorSO3Ptr) rw::core::Ptr<Interpolator<rw::math::Rotation3D<double> > >;
%template (InterpolatorSE3Ptr) rw::core::Ptr<Interpolator<rw::math::Transform3D<double> > >;
%template (InterpolatorQPtr) rw::core::Ptr<Interpolator<rw::math::Q> >;

OWNEDPTR(Interpolator<double> )
OWNEDPTR(Interpolator<rw::math::Vector2D<double> > )
OWNEDPTR(Interpolator<rw::math::Vector3D<double> > )
OWNEDPTR(Interpolator<rw::math::Rotation3D<double> > )
OWNEDPTR(Interpolator<rw::math::Transform3D<double> > )
OWNEDPTR(Interpolator<rw::math::Q> )

class LinearInterpolator: public Interpolator<double> {
public:
    LinearInterpolator(const double& start,
                          const double& end,
                          double duration);

    virtual ~LinearInterpolator();

    double x(double t) const;
    double dx(double t) const;
    double ddx(double t) const;
    double duration() const;
};


class LinearInterpolatorQ: public Interpolator<rw::math::Q> {
public:
    LinearInterpolatorQ(const rw::math::Q& start,
                          const rw::math::Q& end,
                          double duration);

    virtual ~LinearInterpolatorQ();

    rw::math::Q x(double t) const;
    rw::math::Q dx(double t) const;
    rw::math::Q ddx(double t) const;
    double duration() const;
};

class LinearInterpolatorR3: public Interpolator<rw::math::Rotation3D<double> > {
public:
    LinearInterpolatorR3(const rw::math::Rotation3D<double> & start,
                          const rw::math::Rotation3D<double> & end,
                          double duration);

    rw::math::Rotation3D<double>  x(double t) const;
    rw::math::Rotation3D<double>  dx(double t) const;
    rw::math::Rotation3D<double>  ddx(double t) const;
    double duration() const;
};

class LinearInterpolatorSO3: public Interpolator<rw::math::Rotation3D<double> > {
public:
    LinearInterpolatorSO3(const rw::math::Rotation3D<double> & start,
                          const rw::math::Rotation3D<double> & end,
                          double duration);

    rw::math::Rotation3D<double>  x(double t) const;
    rw::math::Rotation3D<double>  dx(double t) const;
    rw::math::Rotation3D<double>  ddx(double t) const;
    double duration() const;
};

class LinearInterpolatorSE3: public Interpolator<rw::math::Transform3D<double> > {
public:
    LinearInterpolatorSE3(const rw::math::Transform3D<double> & start,
                          const rw::math::Transform3D<double> & end,
                          double duration);

    rw::math::Transform3D<double>  x(double t) const;
    rw::math::Transform3D<double>  dx(double t) const;
    rw::math::Transform3D<double>  ddx(double t) const;
    double duration() const;
};


//////////// RAMP interpolator


class RampInterpolatorR3: public Interpolator<rw::math::Vector3D<double> > {
public:
    RampInterpolatorR3(const rw::math::Vector3D<double>& start, const rw::math::Vector3D<double>& end,
                       double vellimit,double acclimit);

    rw::math::Vector3D<double> x(double t) const;
    rw::math::Vector3D<double> dx(double t) const;
    rw::math::Vector3D<double> ddx(double t) const;
    double duration() const;
};

class RampInterpolatorSO3: public Interpolator<rw::math::Rotation3D<double> > {
public:
    RampInterpolatorSO3(const rw::math::Rotation3D<double> & start,
                          const rw::math::Rotation3D<double> & end,
                          double vellimit,double acclimit);

    rw::math::Rotation3D<double>  x(double t) const;
    rw::math::Rotation3D<double>  dx(double t) const;
    rw::math::Rotation3D<double>  ddx(double t) const;
    double duration() const;
};

class RampInterpolatorSE3: public Interpolator<rw::math::Transform3D<double> > {
public:
    RampInterpolatorSE3(const rw::math::Transform3D<double> & start,
                          const rw::math::Transform3D<double> & end,
                          double linvellimit,double linacclimit,
                          double angvellimit,double angacclimit);

    rw::math::Transform3D<double>  x(double t) const;
    rw::math::Transform3D<double>  dx(double t) const;
    rw::math::Transform3D<double>  ddx(double t) const;
    double duration() const;
};

class RampInterpolator: public Interpolator<double> {
public:
    RampInterpolator(const double& start, const double& end, const double& vellimits, const double& acclimits);
    //RampInterpolator(const double& start, const double& end, const double& vellimits, const double& acclimits, double duration);

    double x(double t) const;
    double dx(double t) const;
    double ddx(double t) const;
    double duration() const;
};

class RampInterpolatorQ: public Interpolator<rw::math::Q> {
public:
    RampInterpolatorQ(const rw::math::Q& start, const rw::math::Q& end, const rw::math::Q& vellimits, const rw::math::Q& acclimits);
    //RampInterpolatorQ(const rw::math::Q& start, const rw::math::Q& end, const rw::math::Q& vellimits, const rw::math::Q& acclimits, double duration);

    rw::math::Q x(double t) const;
    rw::math::Q dx(double t) const;
    rw::math::Q ddx(double t) const;
    double duration() const;
};



template <class T>
class Trajectory
{
public:
    virtual T x(double t) const = 0;
    virtual T dx(double t) const = 0;
    virtual T ddx(double t) const = 0;
    virtual double duration() const = 0;
    virtual double startTime() const = 0;
    virtual double endTime() const;

    std::vector<T> getPath(double dt, bool uniform = true);
    //virtual typename rw::core::Ptr< TrajectoryIterator<T> > getIterator(double dt = 1) const = 0;

protected:
    /**
     * @brief Construct an empty trajectory
     */
    Trajectory() {};
};

%template (TrajectoryState) Trajectory<State>;
%template (TrajectoryR1) Trajectory<double>;
%template (TrajectoryR2) Trajectory<rw::math::Vector2D<double> >;
%template (TrajectoryR3) Trajectory<rw::math::Vector3D<double> >;
%template (TrajectorySO3) Trajectory<rw::math::Rotation3D<double> >;
%template (TrajectorySE3) Trajectory<rw::math::Transform3D<double> >;
%template (TrajectoryQ) Trajectory<rw::math::Q>;

%template (TrajectoryStatePtr) rw::core::Ptr<Trajectory<State> >;
%template (TrajectoryR1Ptr) rw::core::Ptr<Trajectory<double> >;
%template (TrajectoryR2Ptr) rw::core::Ptr<Trajectory<rw::math::Vector2D<double> > >;
%template (TrajectoryR3Ptr) rw::core::Ptr<Trajectory<rw::math::Vector3D<double> > >;
%template (TrajectorySO3Ptr) rw::core::Ptr<Trajectory<rw::math::Rotation3D<double> > >;
%template (TrajectorySE3Ptr) rw::core::Ptr<Trajectory<rw::math::Transform3D<double> > >;
%template (TrajectoryQPtr) rw::core::Ptr<Trajectory<rw::math::Q> >;

OWNEDPTR(Trajectory<State> )
OWNEDPTR(Trajectory<double> )
OWNEDPTR(Trajectory<rw::math::Vector2D<double> > )
OWNEDPTR(Trajectory<rw::math::Vector3D<double> > )
OWNEDPTR(Trajectory<rw::math::Rotation3D<double> > )
OWNEDPTR(Trajectory<rw::math::Transform3D<double> > )
OWNEDPTR(Trajectory<rw::math::Q> )

template <class T>
class InterpolatorTrajectory: public Trajectory<T> {
public:
    InterpolatorTrajectory(double startTime = 0);
    void add(rw::core::Ptr<Interpolator<T> > interpolator);
    void add(rw::core::Ptr<Blend<T> > blend,
             rw::core::Ptr<Interpolator<T> > interpolator);
    void add(InterpolatorTrajectory<T>* trajectory);
    size_t getSegmentsCount() const;



    //std::pair<rw::core::Ptr<Blend<T> >, rw::core::Ptr<Interpolator<T> > > getSegment(size_t index) const;
};

%template (InterpolatorTrajectoryR1) InterpolatorTrajectory<double>;
%template (InterpolatorTrajectoryR2) InterpolatorTrajectory<rw::math::Vector2D<double> >;
%template (InterpolatorTrajectoryR3) InterpolatorTrajectory<rw::math::Vector3D<double> >;
%template (InterpolatorTrajectorySO3) InterpolatorTrajectory<rw::math::Rotation3D<double> >;
%template (InterpolatorTrajectorySE3) InterpolatorTrajectory<rw::math::Transform3D<double> >;
%template (InterpolatorTrajectoryQ) InterpolatorTrajectory<rw::math::Q>;


/*
class TrajectoryFactory
{
public:
    static rw::core::Ptr<StateTrajectory> makeFixedTrajectory(const State& state, double duration);
    static rw::core::Ptr<QTrajectory> makeFixedTrajectory(const rw::math::Q& q, double duration);
    static rw::core::Ptr<StateTrajectory> makeLinearTrajectory(const Path<Timed<State>>& path);
    static rw::core::Ptr<StateTrajectory> makeLinearTrajectory(const StatePath& path,
        const models::WorkCell& workcell);
    static rw::core::Ptr<StateTrajectory> makeLinearTrajectoryUnitStep(const StatePath& path);
    static rw::core::Ptr<QTrajectory> makeLinearTrajectory(const TimedQPath& path);
    static rw::core::Ptr<QTrajectory> makeLinearTrajectory(const QPath& path, const rw::math::Q& speeds);
    static rw::core::Ptr<QTrajectory> makeLinearTrajectory(const QPath& path, const models::Device& device);
    static rw::core::Ptr<QTrajectory> makeLinearTrajectory(const QPath& path, rw::core::Ptr<QMetric> metric);
    static rw::core::Ptr<Transform3DTrajectory> makeLinearTrajectory(const Transform3DPath& path, const std::vector<double>& times);
    static rw::core::Ptr<Transform3DTrajectory> makeLinearTrajectory(const Transform3DPath& path, const rw::core::Ptr<Transform3DMetric> metric);
    static rw::core::Ptr<StateTrajectory> makeEmptyStateTrajectory();
    static rw::core::Ptr<QTrajectory > makeEmptyQTrajectory();
};

*/
 
/********************************************
 * LUA functions
 ********************************************/


#if defined (SWIGLUA)
%luacode {

-- Group: Lua functions
-- Var: print_to_log
print_to_log = true

-- Var: overrides the global print function
local global_print = print

-- Function: print
--  Forwards the global print functions to the sdurw.print functions
--  whenever print_to_log is defined.
function print(...)
    if print_to_log then
        for i, v in ipairs{...} do
            if i > 1 then rw.writelog("\t") end
            sdurw.writelog(tostring(v))
        end
        sdurw.writelog('\n')
    else
        global_print(...)
    end
end

-- Function:
function reflect( mytableArg )
 local mytable
 if not mytableArg then
  mytable = _G
 else
  mytable = mytableArg
 end
   local a = {} -- all functions
   local b = {} -- all Objects/Tables

 if type(mytable)=="userdata" then
   -- this is a SWIG generated user data, show functions and stuff
   local m = getmetatable( mytable )
   for key,value in pairs( m['.fn'] ) do
      if (key:sub(0, 2)=="__") or (key:sub(0, 1)==".") then
          table.insert(b, key)
      else
          table.insert(a, key)
      end
   end
   table.sort(a)
   table.sort(b)
   print("Object type: \n  " .. m['.type'])

   print("Member Functions:")
   for i,n in ipairs(a) do print("  " .. n .. "(...)") end
   for i,n in ipairs(b) do print("  " .. n .. "(...)") end

 else
   local c = {} -- all constants
   for key,value in pairs( mytable ) do
      -- print(type(value))
      if (type(value)=="function") then
          table.insert(a, key)
      elseif (type(value)=="number") then
          table.insert(c, key)
      else
          table.insert(b, key)
      end
   end
   table.sort(a)
   table.sort(b)
   table.sort(c)
   print("Object type: \n  " .. "Table")

   print("Functions:")
   for i,n in ipairs(a) do print("  " .. n .. "(...)") end
   print("Constants:")
   for i,n in ipairs(c) do print("  " .. n) end
   print("Misc:")
   for i,n in ipairs(b) do print("  " .. n) end


--  print("Metatable:")
--  for key,value in pairs( getmetatable(mytable) ) do
--      print(key);
--      print(value);
--  end

 end
 end

function help( mytable )
   reflect( mytable )
end

local used_ns = {}

function using(ns)
  local ns_found = false
  local ns_name;
  local ns_val;
  for n,v in pairs(_G) do
    if n == ns then
      ns_found = true
      ns_name = n
      ns_val = v
      break
    end
  end
  if not ns_found then
    error("Unknown table: " .. ns)
  else
    if used_ns[ns_name] == nil then
      used_ns[ns_name] = ns_val
      for n,v in pairs(ns_val) do
        if n ~= "string" and n ~= "ownedPtr" then
          if _G[n] ~= nil then
            print("name clash: " .. n .. " is already defined")
          else
            _G[n] = v
          end
        end
      end
    end
  end
end

function ownedPtr(arg)
  local found = false
  for ns_n,ns_v in pairs(used_ns) do
    for n,v in pairs(ns_v) do
      if type(v) ~= "function" and type(v) ~= "number" then
        if string.len(n) >= 4 then
          if string.sub(n, -3) == "Ptr" then
            if getmetatable(arg)[".type"] .. "Ptr" == n then
              return ns_v.ownedPtr(arg)
            end
          end
        end
      end
    end
  end
end

function ownedCPtr(arg)
  local found = false
  for ns_n,ns_v in pairs(used_ns) do
    for n,v in pairs(ns_v) do
      if type(v) ~= "function" and type(v) ~= "number" then
        if string.len(n) >= 5 then
          if string.sub(n, -4) == "CPtr" then
            if getmetatable(arg)[".type"] .. "CPtr" == n then
              return ns_v.ownedCPtr(arg)
            end
          end
        end
      end
    end
  end
end
}
#endif


