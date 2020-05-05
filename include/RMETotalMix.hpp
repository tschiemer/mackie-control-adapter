#ifndef RME_TOTALMIX_H
#define RME_TOTALMIX_H

#include "AbstractControlSurface.hpp"
#include "MidiInterface.hpp"

#include <MidiMessage.h>
#include <RtMidi.h>
#include <time_helpers.h>

#include <cassert>
#include <vector>

namespace RMETotalMixMidiAdapter {

  // typedef struct {
  //   /* Navigation/Control */
  //   void (*shiftChannels)(int);
  //   void (*pageUp)(void);
  //   void (*pageDown)(void);
  //
  //   void (*setSend)(int);
  //   void (*selectSubmix)(void);
  // } RMETotalmixCallbacks_t;

  class AbstractControlSurface;

  class RMETotalMix {

    public:

        enum State_t {
          StateOff,
          StateStarting,
          StateRunning,
          StateStopping
        };

    protected:

      AbstractControlSurface * ControlSurfaceRef = NULL;

      int RMEUnitCount = 0;
      std::vector<MidiInterface*> RMEUnitList = std::vector<MidiInterface*>();

      std::string Name = "";

      State_t State = StateOff;

      timestamp_t LastPing = 0;

      static void didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * rmeTotalMix);

    public:

      RMETotalMix(){
        // do nothing
      }

      RMETotalMix(AbstractControlSurface * controlSurfaceRef, int rmeUnitCount, std::string name = ""){
        assert(controlSurfaceRef != NULL);
        assert(rmeUnitCount > 0);

        ControlSurfaceRef = controlSurfaceRef;
        RMEUnitCount = rmeUnitCount;

        setName(name);
      }

      AbstractControlSurface * getControlSurface(){
        return ControlSurfaceRef;
      }

      void setControlSurface(AbstractControlSurface * controlSurfaceRef){
        assert(controlSurfaceRef != NULL);

        ControlSurfaceRef = controlSurfaceRef;
      }

      int getRMEUnitCount(){
        return RMEUnitCount;
      }

      void setRMEUnitCount(int rmeUnitCount){
        assert( State == StateOff );

        RMEUnitCount = rmeUnitCount;
      }

      std::string getName(){
        return Name;
      }

      void setName(std::string name){
        Name = name;
      }

      State_t getState(){
        return State;
      }

      void start();
      void stop();

      bool isConnected(){
        timestamp_t now = getMicrosecondTimestamp();
        return (now - LastPing < 1000);
      }

        /* Navigation/Control */
      void shiftChannels(int n);

      void pageUp();
      void pageDown();

      void setRelativeSendLevel(int ch, MidiMessage::U14 level);
      void setRelativeMute(int ch, bool state);
      void setRelativeSelect(int ch, bool state);

      void setMasterSendLevel(MidiMessage::U14 level);

      void setAbsoluteHardwareSendLevel(int ch, MidiMessage::U7 level);
      void setAbsoluteSoftwareSendLevel(int ch, MidiMessage::U7 level);
      void setAbsoluteSubmixSendLevel(int ch, MidiMessage::U7 level);

      void selectAbsoluteSubmix(int ch);
  };

}

#endif /* RME_TOTALMIX_H */
