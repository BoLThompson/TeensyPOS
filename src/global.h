#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

enum EntryType {
  Sale,
  Refund
};

enum DocType {
  SA,
  Invoice,
};

namespace Entry {
  extern uint8_t employee;
  extern uint32_t cash;
  extern uint32_t id;
  extern EntryType entryType;
  extern DocType docType;
  //FIXME time

  void init();
};

#endif