#include <ControlSurface/Echo.hpp>

#include <iostream>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {

    AbstractControlSurface * Echo::factory(int argc, char * argv[]){
      return (AbstractControlSurface*)new Echo;
    }

    std::string Echo::getKey(){
      return Key;
    }


    void Echo::setRMETotalMixImpl(RMETotalMix * rmeTotalMix){
      rmeTotalMix->setRMEUnitCount(2);
    }

    void Echo::startImpl(){
      std::cout << "Starting ECHO" << std::endl;
    }
    void Echo::stopImpl(){
      std::cout << "Stopping ECHO" << std::endl;
    }

    void Echo::setSendLevel(int channel, MidiMessage::U14 level){
      std::cout << "Setting Send Level ( channel = " << channel << ", level = " << level << " )" << std::endl;
    }

    void Echo::setMasterLevel(MidiMessage::U14 level){
      std::cout << "Setting Master Level ( level = " << level << " )" << std::endl;
    }

  }
}
