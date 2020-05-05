#ifndef CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H
#define CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H

#include <AbstractControlSurface.hpp>
#include <MidiInterface.hpp>
#include <time_helpers.h>

#include <RtMidi.h>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {



    class Yamaha01V96MIDIRemote : virtual AbstractControlSurface {

      protected:

        MidiInterface * MidiInterfaceRef = NULL;

        int DeviceId = 0;

        // channel 0-15 + master/stereo
        timestamp_t ChannelActivityTimestampList[17];

        int CurrentBank = 0;

        timestamp_t LastPing = 0;

        Yamaha01V96MIDIRemote(int argc, char * argv[]);



        void resetChannelActivityTimestamps();
        // void setChannelActivityTimestamp(int channel){
        //   assert( channel < 17 );
        //   ChannelActivityTimestampList[channel] = ;
        // }
        bool isChannelBlocked(int channel);

        void setRMETotalMixImpl(RMETotalMix * rmeTotalMix);

        static void didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * yamaha01V96MIDIRemote);




        void onConnected();
        void onDisconnected();
        void onPing();

        void queryCurrentSelect();
        void queryCurrentBank();

      public:


        static constexpr const char * Key = "Yamaha01V96-MIDI-Remote";

        static AbstractControlSurface * factory(int argc, char * argv[]);

        std::string getKey();

        int getDeviceId(){
          return DeviceId;
        }

        void setDeviceId(int deviceId){
          assert( deviceId > 0 );
          assert( deviceId < 16 );

          DeviceId = deviceId;
        }

      protected:

        void startImpl();
        void stopImpl();

      public:

        bool isConnected(){
          timestamp_t now = getMicrosecondTimestamp();
          return (now - LastPing < 500000);
        }

        void onRMEPing();
        void onRMEConnected();
        void onRMEDisconnected();

        void setSendLevel(int channel, MidiMessage::U14 level);
        // void didSelectSubmix(int channel);

        void setMasterLevel(MidiMessage::U14 level);

        void selectChannel(int channel);
        void setMuteChannel(int channel, bool mute);
    };

  }
}

#endif /* CONTROL_SURFACE_YAMAHA_01V96_MIDIREMOTE_H */
