#include "./compactador/compactador.h"
#include <iostream>

int main(int, char** argv) {
  if (argv[1][1] == 'd') {
    Compactador::descompactar(argv[2], argv[3]);
    std::cout << "concluido com sucesso" << std::endl;
  } else {
    Compactador::compactar(argv[2], argv[3]);
    std::cout << "concluido com sucesso" << std::endl;
  }

  return 0;
}