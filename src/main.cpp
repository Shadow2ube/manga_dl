#include <iostream>
#include <string>
#include "mangapark.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Invalid arguments\nmanga-dl <full-url>" << std::endl;
  }

  for (int i = 1; i < argc; ++i) {
    std::string full_url = argv[i];
    if (full_url.find("mangapark") != std::string::npos) {
      download_from_mangapark(full_url, i - 1);
    }
  }
  return 0;
}
