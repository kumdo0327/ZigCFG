#include "CommandLineInterface.h"

int main(int argc, char *argv[]) {
  CommandLineInterface Cli(argc, argv);
  Cli.run();
  return 0;
}