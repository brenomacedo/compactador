#pragma once

#include <vector>

#define pai(x) (((x) - 1) >> 1) 
#define esq(x) (((x) << 1) + 1)
#define dir(x) (((x) + 1) << 1)

template <typename T>
class Heap {
    public:
        int tamanho();
        void inserir(T valor);
        T extrairMin();
        void printarElementos();

        Heap();
        Heap(T* vetor, int num_elementos, int tamanho_vetor);
        ~Heap();

    private:
        void criarHeap(T* vetor, int tamanho);
        void subir(int n);
        void descer(int n);
        void aumentarVetor();

        T* heap_vetor;
        int tamanho_heap_vetor = 16;
        
        int tamanho_heap = 0;
};