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

    void Yamaha01V96MIDIRemote::resetChannelActivityTimestamps(){
      for(int i = 0; i < 17; i++){
        ChannelActivityTimestampList[i] = 0;
      }
    }

    bool Yamaha01V96MIDIRemote::isChannelBlocked(int channel){
      timestamp_t now = getMicrosecondTimestamp();
      timestamp_t delta = now - ChannelActivityTimestampList[channel];
      // if (delta < 10000){
      //   std::cout << "blocked " << delta << std::endl;
      // }
      return (delta < 10000);
      // std::time_t now = std::time(nullptr);
    }

    void Yamaha01V96MIDIRemote::setRMETotalMixImpl(RMETotalMix * rmeTotalMix){
      rmeTotalMix->setRMEUnitCount(2);
    };

    void Yamaha01V96MIDIRemote::didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * yamaha01V96MIDIRemote){
      assert( message != NULL );
      assert( midiInterface != NULL );
      assert( yamaha01V96MIDIRemote != NULL );

      Yamaha01V96MIDIRemote * self = (Yamaha01V96MIDIRemote*)yamaha01V96MIDIRemote;


      // convert message format to a more useful format.
      std::string msg( message->begin(), message->end() );

      uint8_t bytes[128];
      memcpy(bytes, msg.c_str(), msg.size());


      // is it a mixer ping?
      //f0 43 10 3e d 7f f7
      static uint8_t Ping[] = {0xF0, 0x43, 0x00, 0x3E, 0x0D, 0x7F, 0xF7};
      Ping[2] = (0x10 | self->DeviceId);

      // std::cout << "> len=" << sizeof(Ping) << " ";
      //       // debug output
      //       for (int i = 0; i < sizeof(Ping); i++){
      //         std::cout << std::hex << (int)Ping[i] << " ";
      //       }
      //       std::cout << std::endl;

      if (message->size() == sizeof(Ping) && std::memcmp(Ping, bytes, sizeof(Ping)) == 0 ){

        if (self->isConnected() == false){
          self->onConnected();
        }

        self->LastPing = getMicrosecondTimestamp();

        self->onPing();
      }


      // basic filter by message length
      if( message->size() != 14 && message->size() != 3){
        return;
      }


            // std::cout << "< len=" << message->size() << " ";
            //       // debug output
            //       for (int i = 0; i < message->size(); i++){
            //         std::cout << std::hex << (int)message->at(i) << " ";
            //       }
            //       std::cout << std::endl;




      if (message->size() == 3 && bytes[0] == MidiMessage::StatusClassNoteOn && bytes[2] == 0x7F){
        if (bytes[1] == 46){
          self->RMETotalMixRef->shiftChannels(-8);
        }
        else if (bytes[1] == 47){
          self->RMETotalMixRef->shiftChannels(8);
        }
        else if (bytes[1] == 96){
          self->RMETotalMixRef->pageUp();
        }
        else if (bytes[1] == 97){
          self->RMETotalMixRef->pageDown();
        }
      }

      // {, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive}
      if (bytes[0] != MidiMessage::SystemMessageSystemExclusive) return;
      if (bytes[1] != 0x43) return; // Yamaha manufactuer id
      if (bytes[2] != (0x10 | self->DeviceId)) return;
      if (bytes[3] != 0x3E) return;

      static const uint8_t RemoteLayer[] = {0x7F, 0x01, 0x5B};
      static const uint8_t StereoFader[] = {0x7F, 0x01, 0x4F, 0x00};
      static const uint8_t SelectChannel[] = {0x0D, 0x04, 0x09, 0x19, 0x00, 0x00, 0x00, 0x00};

      // remote layer
      if (std::memcmp(&bytes[4], RemoteLayer, sizeof(RemoteLayer)) == 0){

        // mute on/off
        if (0x00 <= bytes[7] && bytes[7] <= 0x0F){
          int ch = bytes[7];
          int value = bytes[12];

          // dont do anything yet.

        }

        // fader movement
        else if (0x18 <= bytes[7] && bytes[7] <= 0x27){
          int ch = bytes[7] - 0x18;
          int value = bytes[12];
          value = (value << 7) | value;

          // update timestamp BEFORE sending to rme
          self->ChannelActivityTimestampList[ch] = getMicrosecondTimestamp();

          self->RMETotalMixRef->setRelativeSendLevel(ch, value);

        }

        // bank change
        else if (bytes[7] == 0x49){
          int bank = bytes[12];

          self->CurrentBank = bank;

          self->queryCurrentSelect();
          // std::cerr << "Selected Bank " << bank << std::endl;
        }
      }

      else if (std::memcmp(&bytes[4], StereoFader, sizeof(StereoFader)) == 0){
        int value = (bytes[11] << 7) | bytes[12];
        std::cout << "Master " << value << std::endl;

        // 10bit resolution to 14bit resolution
        value <<= 4;

        // update timestamp BEFORE sending to rme
        self->ChannelActivityTimestampList[16] = getMicrosecondTimestamp();

        // self->RMETotalMixRef->setMasterSendLevel(value);
      }

      // select
      else if (std::memcmp(&bytes[4], SelectChannel, sizeof(SelectChannel)) == 0){
        int ch = bytes[12] + 16 * self->CurrentBank;

        self->RMETotalMixRef->selectAbsoluteSubmix(ch);
      }

    }


    std::string Yamaha01V96MIDIRemote::getKey(){
      return Key;
    }


    void Yamaha01V96MIDIRemote::startImpl(){

      resetChannelActivityTimestamps();

      std::string portName;
      if (Name != ""){
        portName = Name + " / 01V96";
      } else {
        portName = "01V96";
      }

      MidiInterfaceRef = new MidiInterface( didReceiveMessageCallback, 0, portName, this);

      MidiInterfaceRef->start();

    }

    void Yamaha01V96MIDIRemote::stopImpl(){

      MidiInterfaceRef->stop();

      delete MidiInterfaceRef;

      MidiInterfaceRef = NULL;

    }


    void Yamaha01V96MIDIRemote::onRMEConnected(){
      std::cerr << "RME Connected" << std::endl;
    }
    void Yamaha01V96MIDIRemote::onRMEDisconnected(){
      std::cerr << "RME Disconnected" << std::endl;
    }

    void Yamaha01V96MIDIRemote::onRMEPing(){
      // std::cout << "RME Ping" << std::endl;
    }

    void Yamaha01V96MIDIRemote::onConnected(){
      std::cerr << "01v96 Connected" << std::endl;

      queryCurrentBank();
      // queryCurrentSelect();
    }

    void Yamaha01V96MIDIRemote::onDisconnected(){
      std::cerr << "01v96 Disconnected" << std::endl;
    }

    void Yamaha01V96MIDIRemote::onPing(){
      // std::cout << "." << std::flush;
    }

    void Yamaha01V96MIDIRemote::queryCurrentSelect(){

        uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x30, 0x3E, 0x0D, 0x04, 0x09, 0x19, 0x00, MidiMessage::SystemMessageEndOfExclusive};

        bytes[2] |= DeviceId;

        MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

    void Yamaha01V96MIDIRemote::queryCurrentBank(){
      uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x30, 0x3E, 0x7F, 0x01, 0x5B, 0x49, 0x00, MidiMessage::SystemMessageEndOfExclusive};

      bytes[2] |= DeviceId;

      MidiInterfaceRef->send(bytes, sizeof(bytes));
    }


    void Yamaha01V96MIDIRemote::setSendLevel(int channel, MidiMessage::U14 level){
      // std::cout << "01v96.setSendLevel " << channel << " " << level << std::endl;
      // do not forward message if fader was before within X usec
      // Totalmix does echo any incoming messages which can lead to unwanted fader blocking
      if (isChannelBlocked(channel)){
        // std::cout << "BLOCKED" << std::endl;
        return;
      }

      uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive};

      bytes[2] |= DeviceId;
      bytes[7] = channel + 0x18;
      bytes[12]= level >> 7;

      MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

    void Yamaha01V96MIDIRemote::setMasterLevel(MidiMessage::U14 level){
      // std::cout << "01v96.setMasterLevel " << level << std::endl;
      if (isChannelBlocked(16)){
        return;
      }

      uint8_t bytes[] = {MidiMessage::SystemMessageSystemExclusive, 0x43, 0x10, 0x3E, 0x7F, 0x01, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MidiMessage::SystemMessageEndOfExclusive};

      bytes[2] |= DeviceId;

      // 14bit to 10bit
      level >>= 4;

      bytes[11] = level >> 7;
      bytes[12] = level & 0x3F;

      MidiInterfaceRef->send(bytes, sizeof(bytes));
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
