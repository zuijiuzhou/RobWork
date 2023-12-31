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

#include "ShowLog.hpp"

#include <rw/core/Log.hpp>
#include <rw/core/LogWriter.hpp>
#include <rw/core/StringUtil.hpp>
#include <rws/RobWorkStudio.hpp>

#include <QApplication>
#include <QEvent>
#include <QTextEdit>
#include <boost/bind/bind.hpp>
#include <iomanip>
#include <sstream>

#define MESSAGE_ADDED_EVENT 2345

using namespace rw::graphics;
using namespace rw::core;
using namespace rws;

//----------------------------------------------------------------------
// Standard plugin methods

class WriterWrapper : public LogWriter
{
  public:
    typedef std::pair< std::string, QColor > Message;
    WriterWrapper (ShowLog* slog, QColor color) : _slog (slog), _color (color), _tabLevel (4) {}

    virtual ~WriterWrapper () {}

    std::vector< std::pair< std::string, QColor > > _msgQueue;

  protected:
    /**
     * @copydoc LogWriter::doFlush
     */
    virtual void doFlush () {}

    /**
     * @copydoc LogWriter::doWrite
     */
    virtual void doWrite (const std::string& input)
    {
        std::stringstream sstr;
        sstr << std::setw (_tabLevel) << std::setfill (' ');
        sstr << input;

        _msgQueue.push_back (Message (sstr.str ().c_str (), _color));
        QApplication::postEvent (_slog, new QEvent ((QEvent::Type) MESSAGE_ADDED_EVENT));
    }

    /**
     * @copydoc LogWriter::doSetTabLevel
     */
    void doSetTabLevel (int tabLevel) { _tabLevel = tabLevel; }

  private:
    ShowLog* _slog;
    QColor _color;
    int _tabLevel;
};

QIcon ShowLog::getIcon ()
{
    //  Q_INIT_RESOURCE(resources);
    return QIcon (":/log.png");
}

ShowLog::ShowLog () : RobWorkStudioPlugin ("Log", getIcon ())
{
    _editor = new QTextEdit (this);
    _editor->setReadOnly (true);
    _editor->setCurrentFont (QFont ("Courier New", 10));

    _endCursor  = new QTextCursor ();
    *_endCursor = _editor->textCursor ();

    setWidget (_editor);    // Sets the widget on the QDockWidget

    _writers.push_back (ownedPtr (new WriterWrapper (this, Qt::black)));
    _writers.push_back (ownedPtr (new WriterWrapper (this, Qt::darkYellow)));
    _writers.push_back (ownedPtr (new WriterWrapper (this, Qt::darkRed)));
}

ShowLog::~ShowLog ()
{
    delete _endCursor;
}

bool ShowLog::event (QEvent* event)
{
    if (event->type () == MESSAGE_ADDED_EVENT) {
        for (rw::core::Ptr< WriterWrapper > writer : _writers) {
            for (unsigned int i = 0; i < writer->_msgQueue.size (); i++) {
                write (writer->_msgQueue[i].first, writer->_msgQueue[i].second);
            }
            writer->_msgQueue.clear ();
        }
        return true;
    }
    else {
        event->ignore ();
    }

    return RobWorkStudioPlugin::event (event);
}

void ShowLog::open (rw::models::WorkCell* workcell)
{
    if (workcell == NULL)
        return;

    log ().info () << "WorkCell opened: " << StringUtil::quote (workcell->getName ()) << std::endl;
}

void ShowLog::close ()
{
    log ().info () << "WorkCell closed!" << std::endl;
}

void ShowLog::receiveMessage (const std::string& plugin, const std::string& id,
                              const rw::core::Message& msg)
{
    RW_WARN ("Deprecated function, use log().info() << \"your string\" instead");
}

void ShowLog::write (const std::string& str, const QColor& color)
{
    _editor->setTextCursor (*_endCursor);
    _editor->setTextColor (color);

    _editor->insertPlainText (str.c_str ());
    *_endCursor = _editor->textCursor ();
}

void ShowLog::frameSelectedListener (rw::kinematics::Frame* frame)
{
    if (frame == NULL)
        return;
    log ().info () << "Frame selected: " << frame->getName () << std::endl;
}

void ShowLog::initialize ()
{
    setParent (getRobWorkStudio ());
    getRobWorkStudio ()->frameSelectedEvent ().add (
        boost::bind (&ShowLog::frameSelectedListener, this, boost::arg< 1 > ()), this);

    log ().setWriterForMask (((Log::AllMask) ^ (Log::WarningMask | Log::DebugMask | Log::FatalMask |
                                                Log::CriticalMask | Log::ErrorMask)),
                             _writers[0]);
    log ().setWriterForMask ((Log::WarningMask | Log::DebugMask), _writers[1]);
    log ().setWriterForMask ((Log::FatalMask | Log::CriticalMask | Log::ErrorMask), _writers[2]);
}

void ShowLog::flush ()
{
    _editor->clear ();
}
