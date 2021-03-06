/**
 * Copyright (c) 20011-2017 Bill Greiman
 * This file is part of the SdFs library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <math.h>
#include "../common/DebugMacros.h"
#include "FatFile.h"
#include "../common/PrintTemplates.h"
//------------------------------------------------------------------------------
static void printHex(print_t* pr, uint8_t w, uint16_t h) {
  char buf[5];
  char* ptr = buf + sizeof(buf);
  *--ptr = 0;
  for (uint8_t i = 0; i < w; i++) {
    char c = h & 0XF;
    *--ptr = c < 10 ? c + '0' : c + 'A' - 10;
    h >>= 4;
  }
  pr->write(ptr);
}
//------------------------------------------------------------------------------
int FatFile::printf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vfprintf(this, fmt, ap);
}
//------------------------------------------------------------------------------
int FatFile::mprintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vmprintf(this, fmt, ap);
}
//------------------------------------------------------------------------------
#if ENABLE_ARDUINO_FEATURES
int FatFile::mprintf(const __FlashStringHelper *ifsh, ...) {
  va_list ap;
  va_start(ap, ifsh);
  return vmprintf(this, ifsh, ap);
}
#endif  // ENABLE_ARDUINO_FEATURES
//------------------------------------------------------------------------------
void FatFile::dmpFile(print_t* pr, uint32_t pos, size_t n) {
  char text[17];
  text[16] = 0;
  if (n >= 0XFFF0) {
    n = 0XFFF0;
  }
  if (!seekSet(pos)) {
    return;
  }
  for (size_t i = 0; i <= n; i++) {
    if ((i & 15) == 0) {
      if (i) {
        pr->write(' ');
        pr->write(text);
        if (i == n) {
          break;
        }
      }
      pr->write('\r');
      pr->write('\n');
      if (i >= n) {
        break;
      }
      printHex(pr, 4, i);
      pr->write(' ');
    }
    int16_t h = read();
    if (h < 0) {
      break;
    }
    pr->write(' ');
    printHex(pr, 2, h);
    text[i&15] = ' ' <= h && h < 0X7F ? h : '.';
  }
  pr->write('\r');
  pr->write('\n');
}
//------------------------------------------------------------------------------
void FatFile::ls(print_t* pr, uint8_t flags, uint8_t indent) {
  FatFile file;
  rewind();
  while (file.openNext(this, O_READ)) {
    // indent for dir level
    if (!file.isHidden() || (flags & LS_A)) {
      for (uint8_t i = 0; i < indent; i++) {
        pr->write(' ');
      }
      if (flags & LS_DATE) {
        file.printModifyDateTime(pr);
        pr->write(' ');
      }
      if (flags & LS_SIZE) {
        file.printFileSize(pr);
        pr->write(' ');
      }
      file.printName(pr);
      if (file.isDir()) {
        pr->write('/');
      }
      pr->write('\r');
      pr->write('\n');
      if ((flags & LS_R) && file.isDir()) {
        file.ls(pr, flags, indent + 2);
      }
    }
    file.close();
  }
}
//------------------------------------------------------------------------------
size_t FatFile::printCreateDateTime(print_t* pr) {
  dir_t dir;
  if (!dirEntry(&dir)) {
    DBG_FAIL_MACRO;
    goto fail;
  }
  return fsPrintDateTime(pr, getLe16(dir.createDate), getLe16(dir.createTime));

fail:
  return 0;
}
//------------------------------------------------------------------------------
size_t FatFile::printModifyDateTime(print_t* pr) {
  dir_t dir;
  if (!dirEntry(&dir)) {
    DBG_FAIL_MACRO;
    goto fail;
  }
  return fsPrintDateTime(pr, getLe16(dir.modifyDate), getLe16(dir.modifyTime));

fail:
  return 0;
}
//------------------------------------------------------------------------------
size_t FatFile::printFileSize(print_t* pr) {
  char buf[11];
  char *ptr = buf + sizeof(buf);
  *--ptr = 0;
  ptr = fmtBase10(ptr, fileSize());
  while (ptr > buf) {
    *--ptr = ' ';
  }
  return pr->write(buf);
}
