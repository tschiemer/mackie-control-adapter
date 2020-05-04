#ifndef MIDI_INTERFACE_H
#define MIDI_INTERFACE_H

#include <RtMidi.h>

namespace RMETotalMixMidiAdapter {

  class MidiInterface;

  typedef void (*MidiInterfaceDidReceiveCallback)(double deltatime, std::vector< unsigned char > *message, MidiInterface * midiInterface, void * context);

  class MidiInterface {

    public:
      enum State_t {
        StateOff,
        StateStarting,
        StateRunning,
        StateStopping
      };

    protected:


      int Id = 0;
      std::string Name = "MIDI";

      RtMidiIn * From = NULL;
      RtMidiOut * To = NULL;

      MidiInterfaceDidReceiveCallback DidReceiveCallback = NULL;

      State_t State = StateOff;

      void * Context = NULL;

      static void rtMidiCallback( double timeStamp, std::vector<unsigned char> *message, void *userData );

    public:

      MidiInterface(){}

      MidiInterface(MidiInterfaceDidReceiveCallback didReceiveCallback, int id = 0, std::string name = "", void * context = NULL){
        assert( didReceiveCallback != NULL );

        Id = id;
        Name = name;
        DidReceiveCallback = didReceiveCallback;
        Context = context;
      }

      ~MidiInterface(){

        // fail if trying to delete while not stopped
        assert( State == StateOff );

        delete From;
        delete To;
      }

      int getId(){
        return Id;
      }

      void setId(int id){
        Id = id;
      }

      std::string getName(){
        return Name;
      }

      void setName(std::string name){
        Name = name;
      }

      MidiInterfaceDidReceiveCallback getDidReceiveCallback(){
        return DidReceiveCallback;
      }

      void setDidReceiveCallback(MidiInterfaceDidReceiveCallback didReceiveCallback){
        assert(didReceiveCallback != NULL);

        DidReceiveCallback = didReceiveCallback;
      }

      void * getContext(){
        return Context;
      }

      void setContext(void * context){
        Context = context;
      }

      State_t getState(){
        return State;
      }

      void start();
      void stop();

      void send( const unsigned char *message, size_t size );
  };

}

#endif /* MIDI_INTERFACE_H */
