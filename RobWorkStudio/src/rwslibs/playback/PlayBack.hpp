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

#ifndef RW_STUDIO_PLAYBACK_MODULE_H
#define RW_STUDIO_PLAYBACK_MODULE_H

#include "PlayBackSettings.hpp"

#include <rw/core/Ptr.hpp>
#include <rw/trajectory/Path.hpp>
#include <rws/RobWorkStudioPlugin.hpp>

class Player;
class StateDraw;

class QCheckBox;
class QLabel;
class QSlider;

namespace rws {

//! @brief Playback plugin for animation of a rw::trajectory::TimedStatePath .
class PlayBack : public RobWorkStudioPlugin
{
    Q_OBJECT
#ifndef RWS_USE_STATIC_LINK_PLUGINS
    Q_INTERFACES (rws::RobWorkStudioPlugin)
    Q_PLUGIN_METADATA (IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "plugin.json")
#endif
  public:
    //! @brief Constructor.
    PlayBack ();

    //! @brief Destructor.
    virtual ~PlayBack ();

    //! @copydoc RobWorkStudioPlugin::open
    virtual void open (rw::models::WorkCell* workcell);

    //! @copydoc RobWorkStudioPlugin::close
    virtual void close ();

    //! @copydoc RobWorkStudioPlugin::initialize
    void initialize ();

  private:
    void stateTrajectoryChangedListener (const rw::trajectory::TimedStatePath::Ptr trajectory);
  private Q_SLOTS:
    void openPath ();
    void savePath ();
    void forwardPlay ();
    void backwardPlay ();
    void pauseOrResumePlay ();
    void toStartPlay ();
    void toEndPlay ();
    void reloadPlay ();
    void sliderSetPosition (int val);
    void relativePositionChanged (double relative);
    void speedValueChanged (double percent);
    void loopPlaybackChanged (int state);
    void interpolateChanged (int state);
    void stateTrajectoryChanged(void);
    void record (bool record);
    void showSettings ();


  Q_SIGNALS:
    void stateTrajectoryChangedSignal(void);


  private:
    class MyStateDraw;
    rw::core::Ptr< StateDraw > makeMyStateDraw ();
    void draw (const rw::kinematics::State& state);

    void csvOpenPlayFile (const std::string& file);
    void rawOpenPlayFile (const std::string& file);
    void openPlayFile (const std::string& file);
    void setInfoLabel ();

    void keyPressEvent (QKeyEvent* event);

  private:
    rw::models::WorkCell* _workcell;
    // rwlibs::drawable::WorkCellGLDrawer* _workcellGLDrawer;

    std::string _previousOpenSaveDirectory;

    rw::core::Ptr< Player > _player;

    QLabel* _info;

    QSlider* _slider;
    bool _inSliderSet;
    bool _inRelativePositionChanged;

    QDoubleSpinBox* _speed;
    QCheckBox *_loop, *_interpolate;

    std::string _file;    // The currently opened file.

    PlayBackSettings _settings;

    QAction* _forward;
    QAction* _pauseAndResume;
    QAction* _backward;
};

}    // namespace rws

#endif
