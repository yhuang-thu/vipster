#ifndef WRAPLAMMPS_H
#define WRAPLAMMPS_H

#include <cstdint>

enum class LMPMessage:uint8_t{CMD, EXIT, ABORT};
enum LMPResult:uint8_t{SUCCESS=0x0, FAIL=0x1, ABORT=0x2};

void launchLammps();

#endif // WRAPLAMMPS_H
