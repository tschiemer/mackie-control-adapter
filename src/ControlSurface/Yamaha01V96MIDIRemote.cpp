#include <ControlSurface/Yamaha01V96MIDIRemote.hpp>

#include <midimessage.h>

#include <iostream>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {

    Yamaha01V96MIDIRemote::Yamaha01V96MIDIRemote(int argc, char * argv[]){
      if (argc == 0){
        return;
      }

      int deviceId = std::atoi(argv[0]);

      setDeviceId( deviceId );
    }

    AbstractControlSurface * Yamaha01V96MIDIRemote::factory(int argc, char * argv[]){
      return new Yamaha01V96MIDIRemote(argc, argv);
    }


    void Yamaha01V96MIDIRemote::setRMETotalMixImpl(RMETotalMix * rmeTotalMix){
      rmeTotalMix->setRMEUnitCount(2);
    };

    void Yamaha01V96MIDIRemote::didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * yamaha01V96MIDIRemote){
      assert( message != NULL );
      assert( midiInterface != NULL );
      assert( yamaha01V96MIDIRemote != NULL );

      if( message->size() != 14 && message->size() != 3){
        return;
      }

      for (int i = 0; i < message->size(); i++){
        std::cout << std::hex << (int)message->at(i) << " ";
      }
      std::cout << std::endl;

      Yamaha01V96MIDIRemote * self = (Yamaha01V96MIDIRemote*)yamaha01V96MIDIRemote;

      std::string msg( message->begin(), message->end() );

      uint8_t bytes[128];

      memcpy(bytes, msg.c_str(), msg.size());

      if (message->size() == 3 && bytes[0] == MidiMessage::StatusClassNoteOn && bytes[2] == 0x7F){
        switch(bytes[1]){
          case 46:
            self->RMETotalMixRef->shiftChannels(-8);
            break;

          case 47:
            self->RMETotalMixRef->shiftChannels(8);
            break;

          case 96:
            self->RMETotalMixRef->pageUp();
            break;

          case 97:
            self->RMETotalMixRef->pageDown();
            break;

          default:
            break;
        }
      }

      // {, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive}
      if (bytes[0] != MidiMessage::SystemMessageSystemExclusive) return;
      if (bytes[1] != 0x43) return; // Yamaha manufactuer id
      if (bytes[2] != (0x10 | self->DeviceId)) return;

      static uint8_t CmdRemoteFader[] = {0x3E, 0x7F, 0x01, 0x5B};
      static uint8_t CmdSelectChannel[] = {0x3E, 0x0D, 0x04, 0x09, 0x19, 0x00, 0x00, 0x00, 0x00};
      static uint8_t CmdMuteChannel[] = {0x3E, 0x7F, 0x01, 0x5B};

      //it's a fader
      if (std::memcmp(&bytes[3], CmdRemoteFader, sizeof(CmdRemoteFader)) == 0 && 0x18 <= bytes[7] && bytes[7] <= 0x27){
        int ch = bytes[7] - 0x18;
        int value = bytes[12];
        value = (value << 7) | value;

        self->RMETotalMixRef->setRelativeSendLevel(ch, value);
      }
      else if (std::memcmp(&bytes[3], CmdSelectChannel, sizeof(CmdSelectChannel)) == 0){
        int ch = bytes[12];

        self->RMETotalMixRef->selectAbsoluteSubmix(ch);

        // self->RMETotalMixRef->setRelativeSelect(ch, true);
      }
      else if (std::memcmp(&bytes[3], CmdMuteChannel, sizeof(CmdMuteChannel)) == 0){
        int ch = bytes[7];
        int value = bytes[12];

        // dont do anything yet.
        // self->RMETotalMixRef->setRelativeSendLevel(ch, value);
      }
    }

    std::string Yamaha01V96MIDIRemote::getKey(){
      return Key;
    }


    void Yamaha01V96MIDIRemote::startImpl(){

      std::string portName;
      if (Name != ""){
        portName = Name + " / 01V96 ";
      } else {
        portName = "01V96 ";
      }

      MidiInterfaceRef = new MidiInterface( didReceiveMessageCallback, 0, portName, this);

      MidiInterfaceRef->start();

    }

    void Yamaha01V96MIDIRemote::stopImpl(){

      MidiInterfaceRef->stop();

      delete MidiInterfaceRef;

      MidiInterfaceRef = NULL;

    }

    void Yamaha01V96MIDIRemote::setSendLevel(int channel, MidiMessage::U14 level){

        uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive};

        bytes[2] |= DeviceId;
        bytes[7] = channel + 0x18;
        bytes[12]= level >> 7;

        MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

    void Yamaha01V96MIDIRemote::setMasterLevel(MidiMessage::U14 level){
      // not implemented
    }

    void Yamaha01V96MIDIRemote::selectChannel(int channel){
      // 10 3E 0D 04 09 19 00 00 00 00 0
      uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x10, 0x3E, 0x0D, 0x04, 0x09, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive};

      bytes[2] |= DeviceId;
      bytes[12] = channel;

      MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

    void Yamaha01V96MIDIRemote::setMuteChannel(int channel, bool mute){
      // 10 3E 7F 01 5B 0([0-9A-F]{1})000000000([01]{1})
      uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive};

      bytes[2] |= DeviceId;
      bytes[7] = channel;
      bytes[12] = mute ? 1 : 0;

      MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

  }
}
