#include <iostream>
#include "./heap/heap.impl.h"

int main() {
  int* teste = new int[9];

  teste[0] = 5;
  teste[1] = 2;
  teste[2] = 1;
  teste[3] = 4;
  teste[4] = 9;
  teste[5] = 16;
  teste[6] = 15;
  teste[7] = 12;
  teste[8] = 0;

  Heap<int> heap = Heap(teste, 9, 9);

  heap.printarElementos();
  std::cout << "extraido: " << heap.extrairMin() << '\n';

  heap.inserir(2);
  heap.printarElementos();
  heap.inserir(6);
  heap.printarElementos();
  heap.inserir(4);
  heap.printarElementos();
  heap.inserir(1);
  heap.printarElementos();
  heap.inserir(0);
  heap.printarElementos();

  return 0;
}