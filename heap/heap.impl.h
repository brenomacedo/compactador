#pragma once

#include <exception>
#include <iostream>
#include "./heap.h"

template <typename T>
int Heap<T>::tamanho() {
  return this->tamanho_heap;
}

template <typename T>
void Heap<T>::inserir(T valor) {
  if (this->tamanho_heap == this->tamanho_heap_vetor) {
      this->aumentarVetor();
  }

  int posicao_inserida = this->tamanho_heap;
  this->tamanho_heap++;
  this->heap_vetor[posicao_inserida] = valor;
  this->subir(posicao_inserida);
}

template <typename T>
T Heap<T>::extrairMin() {
  if (this->tamanho_heap == 0) {
      throw std::underflow_error("Tentando extrair de um heap vazio!");
  }

  T valorExtraido = this->heap_vetor[0];
  this->heap_vetor[0] = this->heap_vetor[this->tamanho_heap - 1];

  this->tamanho_heap--;
  this->descer(0);

  return valorExtraido;
}

template <typename T>
void Heap<T>::aumentarVetor() {
  int tamanho_atual = this->tamanho_heap;
  int novoTamanho = this->tamanho_heap << 1;
  T* new_heap_vetor = new T[novoTamanho];

  for (int i = 0; i < tamanho_atual; i++) {
      new_heap_vetor[i] = this->heap_vetor[i];
  }

  this->tamanho_heap_vetor = novoTamanho;

  delete this->heap_vetor;
  this->heap_vetor = new_heap_vetor;
}

template <typename T>
void Heap<T>::subir(int n) {
  int indice_pai = pai(n);
  while (n != 0 && this->heap_vetor[n] < this->heap_vetor[indice_pai]) {
    T aux = this->heap_vetor[n];
    this->heap_vetor[n] = this->heap_vetor[indice_pai];
    this->heap_vetor[indice_pai] = aux;

    n = indice_pai;
    indice_pai = pai(n);
  }
}

template <typename T>
void Heap<T>::descer(int n) {
  bool descer = true;
  
  while (descer) {
    int esq = esq(n);
    int dir = dir(n);
    int menor = n;

    if (esq < this->tamanho_heap && this->heap_vetor[esq] < this->heap_vetor[menor]) {
      menor = esq;
    }

    if (dir < this->tamanho_heap && this->heap_vetor[dir] < this->heap_vetor[menor]) {
      menor = dir;
    }

    if (menor != n) {
      T aux = this->heap_vetor[menor];
      this->heap_vetor[menor] = this->heap_vetor[n];
      this->heap_vetor[n] = aux;

      n = menor;
    } else {
      descer = false;
    }
  }
}

template <typename T>
void Heap<T>::criarHeap(T* vetor, int tamanho) {
  for (int i = (tamanho >> 1); i >= 0 ; i--) {
    this->descer(i);
  }
}

template <typename T>
void Heap<T>::printarElementos() {
  std::cout << "[ ";
  for (int i = 0; i < this->tamanho_heap; i++) {
    std::cout << this->heap_vetor[i] << " ";
  }
  std::cout << "]\n";
}

template <typename T>
Heap<T>::Heap() : heap_vetor(new T[16]) {}

template <typename T>
Heap<T>::Heap(T* vetor, int num_elementos, int tamanho_vetor) {
  this->heap_vetor = vetor;
  this->tamanho_heap = num_elementos;
  this->tamanho_heap_vetor = tamanho_vetor;

  this->criarHeap(vetor, num_elementos);
}

template <typename T>
Heap<T>::~Heap() {
  delete[] this->heap_vetor;
}