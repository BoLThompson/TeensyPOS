#include "global.h"

uint8_t Entry::employee;
uint32_t Entry::cash;
uint32_t Entry::id;
EntryType Entry::entryType;
DocType Entry::docType;

void Entry::init() {
  uint8_t employee = 0;
  uint32_t cash = 0;
  uint32_t id = 0;
  EntryType entryType = Sale;
  DocType docType = SA;
}