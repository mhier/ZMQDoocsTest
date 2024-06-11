// SPDX-FileCopyrightText: Deutsches Elektronen-Synchrotron DESY, MSK, ChimeraTK Project <chimeratk-support@desy.de>
// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <ChimeraTK/ApplicationCore/ApplicationCore.h>
#include <ChimeraTK/ApplicationCore/PeriodicTrigger.h>

namespace ctk = ChimeraTK;

class ExampleApp : public ctk::Application {
 public:
  using ctk::Application::Application;
  ~ExampleApp() override;

 private:
  ctk::SetDMapFilePath dmapPath{getName() + ".dmap"};
  ctk::PeriodicTrigger timer{this, "Timer", "Periodic timer for the controller"};
  ctk::DeviceModule oven{this, "oven"};

  struct Sender : public ctk::ApplicationModule {
    using ctk::ApplicationModule::ApplicationModule;

    ctk::ScalarPushInput<size_t> trig{this, "/Timer/tick", "", ""};
    ctk::ScalarOutput<int64_t> out{this, "out", "", ""};

    void mainLoop() override { while(true) {
        readAll();
        for(size_t i=0; i<2; ++i) {
          ++out;
          auto fail = out.write();
          if(fail) {
            std::cout << "DATA LOST" << std::endl;
          }
          //usleep(10000);
        }
      }
     }

  } sender{this, "Sender", ""};

  struct Receiver : public ctk::ApplicationModule {
    using ctk::ApplicationModule::ApplicationModule;

    ctk::ScalarPushInput<int64_t> in{this, "in", "", ""};

    void mainLoop() override {
      int32_t expect = 0;
      while(true) {
        readAll();
        ++expect;
        if(in != expect) {
          std::cout << size_t(in) << " != " << expect << std::endl;
          expect = in;
        }
      }
    }

  } receiver{this, "Receiver", ""};
};
