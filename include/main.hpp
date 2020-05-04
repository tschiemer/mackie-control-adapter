#ifndef MAIN_H
#define MAIN_H

#include "RMETotalMix.hpp"

#include "AbstractControlSurface.hpp"
#include "ControlSurface/Echo.hpp"
#include "ControlSurface/Yamaha01V96MIDIRemote.hpp"

typedef struct {
  std::string Key;
  RMETotalMixMidiAdapter::ControlSurfaceFactory Factory;
} ControlSurfaceRegistryEntry_t;


ControlSurfaceRegistryEntry_t ControlSurfaceList[] = {
  {RMETotalMixMidiAdapter::ControlSurface::Echo::Key, RMETotalMixMidiAdapter::ControlSurface::Echo::factory},
  {RMETotalMixMidiAdapter::ControlSurface::Yamaha01V96MIDIRemote::Key, RMETotalMixMidiAdapter::ControlSurface::Yamaha01V96MIDIRemote::factory},
};

const int ControlSurfaceCount = sizeof(ControlSurfaceList) / sizeof(ControlSurfaceRegistryEntry_t);

#endif /* MAIN_H */
