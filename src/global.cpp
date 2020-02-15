#include "global.h"

uint32_t Entry::employee;
uint32_t Entry::cash;
uint32_t Entry::docId;
EntryType Entry::entryType;
DocType Entry::docType;

void Entry::init() {
  employee = 0;
  cash = 0;
  docId = 0;
  entryType = Sale;
  docType = Invoice;
}