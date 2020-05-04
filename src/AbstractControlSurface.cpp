#include <AbstractControlSurface.hpp>

namespace RMETotalMixMidiAdapter {

  //  AbstractControlSurface::AbstractControlSurface(){
  //   RMETotalMixRef = NULL;
  //   Name = "";
  //   State = StateOff;
  // }

  void AbstractControlSurface::setRMETotalMix(RMETotalMix * rmeTotalMix){
    assert( State == StateOff );

    RMETotalMixRef = rmeTotalMix;

    RMETotalMixRef->setControlSurface(this);

    setRMETotalMixImpl(rmeTotalMix);
  }

}
