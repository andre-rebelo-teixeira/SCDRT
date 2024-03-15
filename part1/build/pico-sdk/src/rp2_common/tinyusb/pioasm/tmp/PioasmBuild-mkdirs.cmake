# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/andret/MEEC/SCDTR/Project/pico-sdk/tools/pioasm"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pioasm"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/tmp"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src"
  "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/andret/MEEC/SCDTR/Project/part1/build/pico-sdk/src/rp2_common/tinyusb/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
