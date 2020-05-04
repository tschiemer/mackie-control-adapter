#ifndef CONTROL_SURFACE_H
#define CONTROL_SURFACE_H

#include "RMETotalMix.hpp"

#include <cassert>
#include <string>

#include <MidiMessage.h>

namespace RMETotalMixMidiAdapter {

  class RMETotalMix;
  class AbstractControlSurface;

  typedef AbstractControlSurface* (*ControlSurfaceFactory)(int argc, char * argv[]);

  class AbstractControlSurface {

    public:

      enum State_t {
        StateOff,
        StateStarting,
        StateRunning,
        StateStopping
      };

    protected:

      RMETotalMix * RMETotalMixRef = NULL;

      std::string Name = "";

      State_t State = StateOff;

      // AbstractControlSurface();

    public:

      virtual ~AbstractControlSurface(){};

      RMETotalMix * getRMETotalMix() { return RMETotalMixRef; };

      void setRMETotalMix(RMETotalMix * rmeTotalMix);

    protected:

      virtual void setRMETotalMixImpl(RMETotalMix * rmeTotalMix){};

    public:

      std::string getName(){
        return Name;
      }

      void setName(std::string name){
        Name = name;
      }

      State_t getState(){
        return State;
      }

      void start(){
        assert( RMETotalMixRef != NULL );
        assert( State == StateOff );

        State = StateStarting;

        startImpl();

        State = StateRunning;
      }

      void stop(){

        if (State == StateOff){
          return;
        }

        // if is in transitory state, fail (do not handle but alert developer).
        assert(State == StateRunning);

        State = StateStopping;

        stopImpl();

        State = StateOff;
      }

    protected:

      virtual void startImpl(){};
      virtual void stopImpl(){};

    public:

      virtual std::string getKey() { return "Abstract Control Interface"; };

      virtual void setSendLevel(int channel, MidiMessage::U14 level) {};
      // virtual void didSelectSubmix(int channel) {};

      virtual void setMasterLevel(MidiMessage::U14 level){};


  };

}

#endif /* CONTROL_SURFACE_H */
