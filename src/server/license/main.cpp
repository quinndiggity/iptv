#include <string.h>

#include <iostream>

#include <common/macros.h>

#include "server/license/gen_hardware_hash.h"

#define HELP_TEXT                          \
  "Usage: " PROJECT_NAME                   \
  " [options]\n"                           \
  "  License generation for " PROJECT_NAME \
  " project.\n\n"                          \
  "    --hdd [default] hash algorithm\n"   \
  "    --machine-id hash algorithm\n"

int main(int argc, char** argv) {
  UNUSED(argc);
  UNUSED(argv);
  iptv_cloud::license::ALGO_TYPE algo = iptv_cloud::license::HDD;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--hdd") == 0) {
      algo = iptv_cloud::license::HDD;
    } else if (strcmp(argv[i], "--machine-id") == 0) {
      algo = iptv_cloud::license::MACHINE_ID;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      std::cout << HELP_TEXT << std::endl;
      return EXIT_SUCCESS;
    } else {
      std::cout << HELP_TEXT << std::endl;
      return EXIT_SUCCESS;
    }
  }

  std::string hash;
  if (!iptv_cloud::license::GenerateHardwareHash(algo, &hash)) {
    return EXIT_FAILURE;
  }

  std::cout << hash << std::endl;
  return EXIT_SUCCESS;
}
