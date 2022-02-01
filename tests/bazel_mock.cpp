
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

int
main(int argc, char** argv)
{

  std::vector<std::string> arguments(argv + 1, argv + argc);
  auto info_iter = std::find(std::begin(arguments), std::end(arguments), "info");
  auto aquery_iter = std::find(std::begin(arguments), std::end(arguments), "aquery");

  if (info_iter != std::end(arguments)) {
    ++info_iter;
    if (info_iter != std::end(arguments)) {
      if (*info_iter == std::string_view("workspace")) {
        std::cout << "/tmp/workspace\n";
        return 0;
      } else if (*info_iter == std::string_view("execution_root")) {
        std::cout << "/tmp/execroot\n";
        return 0;
      } else {
        std::cerr << "fatal error: invalid argument for workspace: invlaid location\n";
        return 1;
      }
    } else {
      std::cerr << "fatal error: invalid argument for workspace: location required\n";
      return 1;
    }
  } else if (aquery_iter != std::end(arguments)) {
    // clang-format off
    std::cout 
      << "{\n"
      << "  \"actions\": [{\n"
      << "    \"targetId\": 1,\n"
      << "    \"actionKey\": \"1f29468d5d54dd866a435f9f256366c6c49194618b04ef7f6a290311c4485271\",\n"
      << "    \"mnemonic\": \"CppCompile\",\n"
      << "    \"arguments\": [\"c++\", \"-c\", \"main.cpp\", \"-o\", \"main.o\"]\n"
      << "  }]\n"
      << "}\n";
    // clang-format on
    return 0;
  } else {
    std::cerr << "fatal error: invalid argument: unknown sub-command\n";
    return 1;
  }

  return 0;
}
