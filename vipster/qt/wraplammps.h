#ifndef WRAPLAMMPS_H
#define WRAPLAMMPS_H

#include <cstdint>

enum class LMPMessage:uint8_t{CMD, ABORT, SUCCESS};

void launchLammps();

#endif // WRAPLAMMPS_H
