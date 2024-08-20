#include <functional>

#include "definitions.hpp"

namespace yasf {

using CreateDefs = std::function<Definitions()>;

struct GeneratorMainLib {
  static int run_main(int argc, char* argv[], const CreateDefs& create_defs);
};

} // namespace yasf

yasf::Definitions create_def();

#define IMPLEMENT_MAIN(create_def)                                             \
  int main(int argc, char* argv[])                                             \
  {                                                                            \
    return yasf::GeneratorMainLib::run_main(argc, argv, create_def);           \
  }
