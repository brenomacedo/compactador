#include <iostream>
#include <type_traits>
#include "./compactador.h"
#include "../heap/heap.impl.h"

// 1 <- -> 0

bool operator < (const ElementoArvore& el1, const ElementoArvore& el2) {
  return el1.peso < el2.peso;
}

TabelaValores::TabelaValores(No* arvore_bytes, uint32_t tamanho) {
  uint16_t raiz = tamanho - 1;
  uint32_t folhas = (tamanho + 1) / 2;
  InfoByte info;
  info.qtd_bits = 0;

  this->percursoPreOrdem(arvore_bytes, raiz, tamanho, folhas, info);
}

void TabelaValores::percursoPreOrdem(No* arvore_bytes, uint16_t indice, uint32_t tamanho, uint32_t folhas, InfoByte info) {
  if (indice < folhas) {
    this->conteudo[arvore_bytes[indice].conteudoNo.caractere] = info;
  } else {
    uint32_t pos_novo_bit = info.qtd_bits % 64;
    uint32_t pos_qword = info.qtd_bits / 64;

    uint64_t mask = 1 << pos_novo_bit;
    
    InfoByte info_dir = info;
    InfoByte info_esq = info;

    info_dir.qtd_bits++;
    info_dir.bits_buff[pos_qword] = SET_BIT_OFF(info_dir.bits_buff[pos_qword], mask);

    info_esq.qtd_bits++;
    info_esq.bits_buff[pos_qword] = SET_BIT_ON(info_esq.bits_buff[pos_qword], mask);

    uint16_t filho_esq = arvore_bytes[indice].conteudoNo.conteudoNoInterno.esq;
    uint16_t filho_dir = arvore_bytes[indice].conteudoNo.conteudoNoInterno.dir;

    this->percursoPreOrdem(arvore_bytes, filho_esq, tamanho, folhas, info_esq);
    this->percursoPreOrdem(arvore_bytes, filho_dir, tamanho, folhas, info_dir);
  }
}

void Compactador::compactar(const char* entrada, const char* saida) {
  uint32_t total_bytes = 0;
  uint8_t contagem[256] = {0};
  std::ifstream ler_arquivo(entrada);

  uint8_t c;
  while ((c = ler_arquivo.get()) != unsigned_eof) {
    total_bytes++;
    contagem[c]++;
  }

  uint32_t quantidade_bytes_diferentes = 0;
  for (int i = 0; i < 256; i++) {
    if (contagem[i] != 0) {
      quantidade_bytes_diferentes++;
    }
  }

  // CASO 1: número de bytes é maior que 1
  if (quantidade_bytes_diferentes > 1) {
    Heap<ElementoArvore> heap;

    int tamanho_vetor = (quantidade_bytes_diferentes << 1) - 1;
    No* arvore_bytes = new No[tamanho_vetor];

    uint32_t bytes_inseridos = 0;
    for (uint32_t i = 0; i < 256; i ++) {
      if (contagem[i] != 0) {
        arvore_bytes[bytes_inseridos].conteudoNo.caractere = i;
        arvore_bytes[bytes_inseridos].peso = contagem[i];

        heap.inserir({ bytes_inseridos, contagem[i] });

        bytes_inseridos++;
      }
    }

    while (heap.tamanho() > 1) {
      ElementoArvore dir = heap.extrairMin();
      ElementoArvore esq = heap.extrairMin();
      uint32_t peso_total = dir.peso + esq.peso;

      arvore_bytes[bytes_inseridos].conteudoNo.conteudoNoInterno.dir = dir.indice;
      arvore_bytes[bytes_inseridos].conteudoNo.conteudoNoInterno.esq = esq.indice;
      arvore_bytes[bytes_inseridos].peso = peso_total;

      ElementoArvore no_interno = {
        bytes_inseridos, peso_total
      };

      heap.inserir(no_interno);

      bytes_inseridos++;
    }

    // voltando para o inicio do arquivo
    ler_arquivo.clear();
    ler_arquivo.seekg(std::ios_base::beg);

    std::ofstream escrever_arquivo(saida);

    // TODO: Escrever o n
    // TODO: Escrever o numero de bytes
    
    TabelaValores tabela(arvore_bytes, bytes_inseridos);
    uint8_t byte_buf;
    uint8_t buf_ptr = 0;
    uint8_t buf_mask = 1;

    uint8_t c;
    while ((c = ler_arquivo.get()) != unsigned_eof) {
      InfoByte info = tabela.conteudo[c];
      
      for (int k = 0; k < info.qtd_bits; k++) {
        uint8_t pos_bit = k % 64;
        uint8_t pos_qword = k / 64;

        
      }
    }

    // CASO 2: número de bytes é igual a 1
  } else if (quantidade_bytes_diferentes == 1) { 

    // CASO 3: número de bytes é 0
  } else {

  }


}
