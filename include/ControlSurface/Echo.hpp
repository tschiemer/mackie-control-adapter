
#include <AbstractControlSurface.hpp>

namespace RMETotalMixMidiAdapter {
  namespace ControlSurface {

    class Echo : AbstractControlSurface {

      protected:

      // Echo() : AbstractControlSurface() {
      //   // do nothing
      // }

      public:

        static constexpr const char * Key = "ECHO";

        static AbstractControlSurface * factory(int argc, char * argv[]);

        std::string getKey();


      protected:

        void setRMETotalMixImpl(RMETotalMix * rmeTotalMix);
        void startImpl();
        void stopImpl();

      public:

        void setSendLevel(int channel, MidiMessage::U14 level);
        // void didSelectSubmix(int channel);

        void setMasterLevel(MidiMessage::U14 level);

    };

  }
}
