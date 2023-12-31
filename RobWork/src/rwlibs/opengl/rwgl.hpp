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

#ifndef RWLIBS_OS_RWGL_HPP
#define RWLIBS_OS_RWGL_HPP

#include <rw/core/os.hpp>

#if defined(RW_CYGWIN)
#include <GL/gl.h>
#include <windows.h>
#include <GL/glext.h>
#include <GL/glu.h>    // Header File For The GLu32 Library
#elif defined(RW_WIN32)
#include <GL/gl.h>
#include <windows.h>
#include <rwlibs/opengl/glext_win32.h>
#include <GL/glu.h>    // Header File For The GLu32 Library
#elif defined(RW_MACOS)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#elif defined(RW_LINUX)
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>    // Header File For The GLu32 Library
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>    // Header File For The GLu32 Library
#endif

#endif /*RWLIBS_OS_RWGL_HPP*/
