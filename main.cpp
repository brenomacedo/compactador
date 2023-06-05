#include <iostream>
#include "./compactador/compactador.h"
#include "./heap/heap.impl.h"

int main() {
  Compactador compactador;
  compactador.compactar("teste.txt", "saida.cmp");

  return 0;
}
