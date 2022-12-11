#!/usr/bin/env python

import os
import sys

Import("env")
FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoteensy")
CORE_PATCHES_DIR = "core_patches"

print("Patching arduino core libs...")

print(os.listdir(FRAMEWORK_DIR))

for file_name in os.listdir(CORE_PATCHES_DIR):
  # Skip this file
  if file_name == 'apply.py':
    continue

  env.Execute(f"cp {CORE_PATCHES_DIR}/{file_name} {FRAMEWORK_DIR}/cores/teensy4/{file_name}")
  print(file_name)

