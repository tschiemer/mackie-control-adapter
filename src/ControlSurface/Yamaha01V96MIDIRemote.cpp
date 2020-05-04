#include <ControlSurface/Yamaha01V96MIDIRemote.hpp>

#include <midimessage.h>

#include <iostream>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {

    Yamaha01V96MIDIRemote::Yamaha01V96MIDIRemote(int argc, char * argv[]){

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

      Yamaha01V96MIDIRemote * self = (Yamaha01V96MIDIRemote*)yamaha01V96MIDIRemote;

      std::string msg( message->begin(), message->end() );
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

        bytes[7] = channel + 0x17;
        bytes[12]= level >> 7;

        MidiInterfaceRef->send(bytes, sizeof(bytes));
    }

    void Yamaha01V96MIDIRemote::setMasterLevel(MidiMessage::U14 level){
      // not implemented
    }

  }
}
