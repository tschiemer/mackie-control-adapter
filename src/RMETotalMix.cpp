#include <RMETotalMix.hpp>
#include <RMETotalMixConstants.h>

#include <midimessage/packers.h>

namespace RMETotalMixMidiAdapter{

  void RMETotalMix::didReceiveMessageCallback(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * rmeTotalMix){
      assert( message != NULL );
      assert( midiInterface != NULL );
      assert( rmeTotalMix != NULL );

      if (message->size() < 3){
        return;
      }

      // // debug output
      // for (int i = 0; i < message->size(); i++){
      //   std::cout << std::hex << (int)message->at(i) << " ";
      // }
      // std::cout << std::endl;

      RMETotalMix * self = (RMETotalMix*)rmeTotalMix;

      std::string msg( message->begin(), message->end() );

      uint8_t bytes[128];

      memcpy(bytes, msg.c_str(), msg.size());

      MidiMessage::U7 note = 0;
      MidiMessage::U7 velocity = 0;
      MidiMessage::U7 channel = 0;
      MidiMessage::U14 level = 0;

      static const constexpr uint8_t Ping[] = {0x9F, 0x7F, 0x5A};

      if (std::memcmp(Ping, bytes, sizeof(Ping)) == 0){

        if (self->isConnected() == false){
          self->ControlSurfaceRef->onRMEConnected();
        }

        self->LastPing = getMicrosecondTimestamp();

        // std::cout << "lastping = " << self->LastPing << std::endl;

        self->ControlSurfaceRef->onRMEPing();
      }

      if (MidiMessage::unpackPitchBendChange(bytes, msg.size(), &channel, &level)){
        if ( 0 <= channel && channel <= 7 ){

          // actual channel depends on extender from which it came
          channel += midiInterface->getId() * 8;

          self->ControlSurfaceRef->setSendLevel(channel, level);
        }
        else if ( channel == 8 ){
          self->ControlSurfaceRef->setMasterLevel(level);
        }
      }
  }

  void RMETotalMix::start(){
    assert( State == StateOff );
    assert( ControlSurfaceRef != NULL ); // don't forget to link control surface
    assert( RMEUnitCount > 0 ); // don't forget to set the required unit count before trying to start..

    State = StateStarting;

    // instantiate all necessary interface objects
    for(int i = 0; i < RMEUnitCount; i++){
      std::string portName;
      if (Name != ""){
        portName = Name + " / RME " + std::to_string(i);
      } else {
        portName = "RME " + std::to_string(i);
      }

      MidiInterface * mi = new MidiInterface( didReceiveMessageCallback, i, portName, this);
      RMEUnitList.push_back(mi);
    }

    // actually start all interfaces
    for(int i = 0; i < RMEUnitList.size(); i++){
      RMEUnitList.at(i)->start();
    }

    State = StateRunning;
  }

  void RMETotalMix::stop(){

    if (State == StateOff){
      return;
    }

    // if is in transitory state, fail (do not handle but alert developer).
    assert(State == StateRunning);

    State = StateStopping;

    while(!RMEUnitList.empty()){
      MidiInterface * mi = RMEUnitList.back();

      mi->stop();

      delete mi;

      RMEUnitList.pop_back();
    }

    State = StateOff;
  }

  void RMETotalMix::shiftChannels(int n){
    if (State != StateRunning){
      return;
    }

    uint8_t bytes[3];

    while (n != 0){
      if (n >= 8){
        MidiMessage::packNoteOff(bytes, NoteChannel, NoteBankRight, 0);
        RMEUnitList.at(0)->send(bytes, sizeof(bytes));
        n -= 8;
      }
      else if (n >= 1){
        MidiMessage::packNoteOff(bytes, NoteChannel, NoteChannelRight, 0);
        RMEUnitList.at(0)->send(bytes, sizeof(bytes));
        n -= 1;
      }
      else if (n <= -8){
        MidiMessage::packNoteOff(bytes, NoteChannel, NoteBankLeft, 0);
        RMEUnitList.at(0)->send(bytes, sizeof(bytes));
        n += 8;
      }
      else if (n <= -1){
        MidiMessage::packNoteOff(bytes, NoteChannel, NoteChannelLeft, 0);
        RMEUnitList.at(0)->send(bytes, sizeof(bytes));
        n += 1;
      }
    }
  }

  void RMETotalMix::pageUp(){
    if (State != StateRunning){
      return;
    }

    uint8_t bytes[3];

    MidiMessage::packNoteOff(bytes, NoteChannel, NotePageUp, 0);

    RMEUnitList.at(0)->send(bytes, sizeof(bytes));
  }

  void RMETotalMix::pageDown(){
    if (State != StateRunning){
      return;
    }

    uint8_t bytes[3];

    MidiMessage::packNoteOff(bytes, NoteChannel, NotePageDown, 0);

    RMEUnitList.at(0)->send(bytes, sizeof(bytes));
  }

  void RMETotalMix::setRelativeSendLevel(int ch, MidiMessage::U14 level){
    if (State != StateRunning){
      return;
    }

    int controlUnit = ch / 8;
    int channelOfUnit = (ch % 8);
    int value = level > MidiMessage::MaxU14 ? MidiMessage::MaxU14 : level;

    assert( controlUnit <= RMEUnitCount );

    uint8_t bytes[3];

    MidiMessage::packPitchBendChange(bytes, channelOfUnit, value);

    RMEUnitList.at(controlUnit)->send(bytes,sizeof(bytes));
  }

  void RMETotalMix::setMasterSendLevel(MidiMessage::U14 level){
    if (State != StateRunning){
      return;
    }

    int controlUnit = 0;
    int channelOfUnit = 9;
    int value = level > MidiMessage::MaxU14 ? MidiMessage::MaxU14 : level;

    assert( controlUnit <= RMEUnitCount );

    uint8_t bytes[3];

    MidiMessage::packPitchBendChange(bytes, channelOfUnit, value);

    RMEUnitList.at(controlUnit)->send(bytes,sizeof(bytes));
  }

  void RMETotalMix::setAbsoluteHardwareSendLevel(int ch, MidiMessage::U7 level){
    if (State != StateRunning){
      return;
    }

    int channel = (ch / 16) + AbsoluteHardwareSendLevelChannelOffset;
    int cc = (ch % 16) + AbsoluteChannelControllerOffset;
    int value = level > MidiMessage::MaxU7 ? MidiMessage::MaxU7 : level;

    uint8_t bytes[3];

    MidiMessage::packControlChange(bytes, channel, cc, value);

    RMEUnitList.at(0)->send(bytes,sizeof(bytes));
  }

  void RMETotalMix::setAbsoluteSoftwareSendLevel(int ch, MidiMessage::U7 level){
    if (State != StateRunning){
      return;
    }

    int channel = (ch / 16) + AbsoluteSoftwareSendLevelChannelOffset;
    int cc = (ch % 16) + AbsoluteChannelControllerOffset;
    int value = level > MidiMessage::MaxU7 ? MidiMessage::MaxU7 : level;

    uint8_t bytes[3];

    MidiMessage::packControlChange(bytes, channel, cc, value);

    RMEUnitList.at(0)->send(bytes,sizeof(bytes));
  }

  void RMETotalMix::setAbsoluteSubmixSendLevel(int ch, MidiMessage::U7 level){
    if (State != StateRunning){
      return;
    }

    int channel = (ch / 16) + AbsoluteSubmixSendLevelChannelOffset;
    int cc = (ch % 16) + AbsoluteChannelControllerOffset;
    int value = level > MidiMessage::MaxU7 ? MidiMessage::MaxU7 : level;

    uint8_t bytes[3];

    MidiMessage::packControlChange(bytes, channel, cc, value);

    RMEUnitList.at(0)->send(bytes,sizeof(bytes));
  }

  void RMETotalMix::selectAbsoluteSubmix(int ch){
    if (State != StateRunning){
      return;
    }

    ch += 2;

    int channel = (ch / 16) + AbsoluteSubmixSelectChannelOffset;
    int cc = (ch % 16) + AbsoluteSubmitSelectControllerOffset;
    int value = 0;

    uint8_t bytes[3];

    MidiMessage::packControlChange(bytes, channel, cc, value);

    RMEUnitList.at(0)->send(bytes,sizeof(bytes));
  }
}
