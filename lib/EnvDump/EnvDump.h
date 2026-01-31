#ifndef ENVDUMP_H
#define ENVDUMP_H

#include <Arduino.h>

class EnvDump
{
public:
  static void print(Stream &out);
};

#endif // ENVDUMP_H
