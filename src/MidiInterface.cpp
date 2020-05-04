#include <MidiInterface.hpp>

#include <cassert>

namespace RMETotalMixMidiAdapter{

  void MidiInterface::rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *userData ){
    assert( userData != NULL );

    MidiInterface * midiInterface = (MidiInterface*)userData;

    // safety
    if (midiInterface->State != StateRunning){
      return;
    }

    assert( midiInterface->DidReceiveCallback != NULL );

    midiInterface->DidReceiveCallback(timeStamp, message, midiInterface, midiInterface->Context);
  }

  // MidiInterface::MidiInterface(int id, std::string name, MidiInterfaceDidReceiveCallback didReceiveCallback){
  //   assert( didReceiveCallback != NULL );
  //
  //   Id = id;
  //   Name = name;
  //   DidReceiveCallback = didReceiveCallback;
  // }


  void MidiInterface::start(){
    assert( State == StateOff );
    assert( DidReceiveCallback != NULL );

    State = StateStarting;

    From = new RtMidiIn();
    From->openVirtualPort("From " + Name);
    From->setCallback( rtMidiCallback, this );
    // Don't ignore sysex, timing, or active sensing messages.
    From->ignoreTypes( false, false, false );


    To = new RtMidiOut();
    To->openVirtualPort("To " + Name);

    State = StateRunning;
  }

  void MidiInterface::stop(){

    if (State == StateOff){
      return;
    }

    // if is in transitory state, fail (do not handle but alert developer).
    assert(State == StateRunning);

    State = StateStopping;

    assert(From != NULL);
    assert(To != NULL);

    From->closePort();
    delete From;
    From = NULL;

    To->closePort();
    delete To;
    To = NULL;

    State = StateOff;
  }

  void MidiInterface::send( const unsigned char *message, size_t size ){
    if (State != StateRunning){
      return;
    }

    To->sendMessage(message, size);
  }
}
