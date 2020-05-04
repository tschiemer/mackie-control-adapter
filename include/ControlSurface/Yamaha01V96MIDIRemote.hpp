#ifndef CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H
#define CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H

#include <AbstractControlSurface.hpp>
#include <MidiInterface.hpp>

#include <RtMidi.h>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {



    class Yamaha01V96MIDIRemote : virtual AbstractControlSurface {

      public:

        // static const constexprt char Channel

      protected:

        MidiInterface * MidiInterfaceRef = NULL;

        Yamaha01V96MIDIRemote(int argc, char * argv[]);

        void setRMETotalMixImpl(RMETotalMix * rmeTotalMix);

        static void didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * yamaha01V96MIDIRemote);


      public:

        static constexpr const char * Key = "Yamaha01V96-MIDI-Remote";

        static AbstractControlSurface * factory(int argc, char * argv[]);

        std::string getKey();


      protected:

        void startImpl();
        void stopImpl();

      public:

        void setSendLevel(int channel, MidiMessage::U14 level);
        // void didSelectSubmix(int channel);

        void setMasterLevel(MidiMessage::U14 level);

    };

  }
}

#endif /* CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H */
