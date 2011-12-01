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


#include "LogStreamWriter.hpp"

#include "macros.hpp"
#include <iomanip>
using namespace rw::common;

LogStreamWriter::LogStreamWriter(std::ostream* stream) :
    _stream(stream),
	_tabLevel(0)
{
   // RW_ASSERT(stream);  
}

LogStreamWriter::~LogStreamWriter()
{
    flush();
}

void LogStreamWriter::write(const std::string& str)
{
	*_stream << std::setw(_tabLevel)<<std::setfill(' ');
    *_stream << str;
}

void LogStreamWriter::flush()
{
    _stream->flush();
}

void LogStreamWriter::setTabLevel(int tabLevel) {
	_tabLevel = tabLevel;
}