/********************************************************************************
 * Copyright 2016 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#include <rw/core/PropertyMap.hpp>
#include <rws/RobWorkStudio.hpp>

#include <QApplication>
#include <gtest/gtest.h>

using namespace rw::core;
using namespace rw::common;
using namespace rws;

TEST (RobWorkStudio, LaunchTest)
{
    int argc      = 1;
    char name[]   = "RobWorkStudio";
    char* argv[1] = {name};
    PropertyMap map;
    QApplication app (argc, argv);
    RobWorkStudio rwstudio (map);
    rwstudio.show ();
    app.processEvents ();
    TimerUtil::sleepMs (1000);
    rwstudio.close ();
    TimerUtil::sleepMs (2000);
}
