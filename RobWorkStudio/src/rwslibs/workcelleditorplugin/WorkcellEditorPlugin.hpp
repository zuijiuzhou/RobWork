/********************************************************************************
 * Copyright 2018 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RWS_PLUGIN_WORKCELLEDITOR_HPP
#define RWS_PLUGIN_WORKCELLEDITOR_HPP

#include <rws/RobWorkStudioPlugin.hpp>

namespace rws {
class WorkcellEditorWindow;

/**
 * @brief this plugin provides access to the workcell editor of RobWorkStudio
 */
class WorkcellEditorPlugin : public RobWorkStudioPlugin
{
    Q_OBJECT
#ifndef RWS_USE_STATIC_LINK_PLUGINS
    Q_INTERFACES (rws::RobWorkStudioPlugin)
    Q_PLUGIN_METADATA (IID "dk.sdu.mip.Robwork.RobWorkStudioPlugin/0.1" FILE "plugin.json")
#endif
  public:
    //! @brief constructor
    WorkcellEditorPlugin ();

    //! @brief destructor
    virtual ~WorkcellEditorPlugin ();

    //! @copydoc RobWorkStudioPlugin::initialize
    void initialize ();

    //! @copydoc RobWorkStudioPlugin::open
    void open (rw::models::WorkCell* workcell);

    //! @copydoc RobWorkStudioPlugin::close
    void close ();

    //! @copydoc RobWorkStudioPlugin::setupMenu
    void setupMenu (QMenu* menu);

  private Q_SLOTS:

    void startEditor ();

  private:
    // This listens for changes to the state of RobWorkStudio.
    void stateChangedListener (const rw::kinematics::State& state);

  private:
    rw::kinematics::State _state;
    std::string _previousOpenDirectory;
    WorkcellEditorWindow* _editor;
    QAction* _openWorkcellEditorAction;
};

}    // namespace rws

#endif
