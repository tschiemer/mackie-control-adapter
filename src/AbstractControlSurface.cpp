#include <AbstractControlSurface.hpp>

namespace RMETotalMixMidiAdapter {


  void AbstractControlSurface::setRMETotalMix(RMETotalMix * rmeTotalMix){
    assert( State == StateOff );

    RMETotalMixRef = rmeTotalMix;

    RMETotalMixRef->setControlSurface(this);

    setRMETotalMixImpl(rmeTotalMix);
  }

}
