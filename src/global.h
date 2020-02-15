#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

#define LINE_HEIGHT 8

enum EntryType {
  Sale,
  Refund
};

enum DocType {
  SA,
  Invoice,
};

namespace Entry {
  extern uint32_t employee;
  extern uint32_t cash;
  extern uint32_t docId;
  extern EntryType entryType;
  extern DocType docType;

  void init();
};

#endif