/*
 * awui/IO/Directory.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Directory.h"

#include <libgen.h>
#include <unistd.h>

using namespace awui;
using namespace awui::IO;

String Directory::GetCurrentDirectory() {
  char path[255];
  char dest[255];
  pid_t pid = getpid();
  sprintf(path, "/proc/%d/exe", pid);
  if (readlink(path, dest, 255) != -1)
    return dirname(dest);

  return "";
}