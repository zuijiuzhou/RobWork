/******************************************************************************
 * Copyright 2020 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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
 ******************************************************************************/

#include <gtest/gtest.h>

#include <rw/core/Event.hpp>

#include <boost/any.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

using namespace rw::core;

using boost::placeholders::_1;
using boost::placeholders::_2;

typedef boost::function<void(const std::string&, boost::any)> GenericAnyEventListener;
typedef rw::core::Event<GenericAnyEventListener, const std::string&, boost::any> GenericAnyEvent;

namespace {
    bool b1,b2;
    void cb1(const std::string& id, boost::any data){
        b1 = true;
    }

    void cb2(const std::string& id, boost::any data){
        b2 = true;
    }

    class AnyEventListener {
    public:
        AnyEventListener(bool &check): _check(check) {}
        void cb(const std::string& id, boost::any data) {
            _check = true;
        }
        bool &_check;
    };
}

TEST( EventTest, GlobalCallbackFunctions ) {
    GenericAnyEvent event;
    event.add( boost::bind(&cb1, _1, _2), (void*)&cb1 );
    event.add( boost::bind(&cb2, _1, _2), (void*)&cb2 );

    const boost::any data;
    b1=false;
    b2=false;
    event.fire("Msg1",data);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);

    // remove cb1
    b1=false;
    b2=false;
    event.remove((void*)&cb1 );
    event.fire("Msg2",data);
    EXPECT_FALSE(b1);
    EXPECT_TRUE(b2);
}

TEST( EventTest, EventListener ) {
    AnyEventListener listener1(b1);
    AnyEventListener listener2(b2);

    GenericAnyEvent event;
    event.add( boost::bind(&AnyEventListener::cb, &listener1, _1, _2), &listener1 );
    event.add( boost::bind(&AnyEventListener::cb, &listener2, _1, _2), &listener2 );

    const boost::any data;
    b1=false;
    b2=false;
    event.fire("Msg1",data);
    EXPECT_TRUE(b1);
    EXPECT_TRUE(b2);

    // remove cb1
    b1=false;
    b2=false;
    event.remove(&listener1);
    event.fire("Msg2",data);
    EXPECT_FALSE(b1);
    EXPECT_TRUE(b2);
}
