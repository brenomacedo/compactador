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
    this->conteudo[arvore_bytes[indice].caractere] = info;
  } else {
    uint32_t pos_novo_bit = info.qtd_bits % 8;
    uint32_t pos_byte = info.qtd_bits / 8;

    uint64_t mask = 1 << pos_novo_bit;
    
    InfoByte info_dir = info;
    InfoByte info_esq = info;

    info_dir.qtd_bits++;
    info_dir.bits_buff[pos_byte] = SET_BIT_OFF(info_dir.bits_buff[pos_byte], mask);

    info_esq.qtd_bits++;
    info_esq.bits_buff[pos_byte] = SET_BIT_ON(info_esq.bits_buff[pos_byte], mask);

    uint16_t filho_esq = arvore_bytes[indice].conteudoNoInterno.esq;
    uint16_t filho_dir = arvore_bytes[indice].conteudoNoInterno.dir;

    this->percursoPreOrdem(arvore_bytes, filho_esq, tamanho, folhas, info_esq);
    this->percursoPreOrdem(arvore_bytes, filho_dir, tamanho, folhas, info_dir);
  }
}

void Compactador::compactar(const char* entrada, const char* saida) {
  uint32_t total_bytes = 0;
  uint8_t contagem[256] = {0};
  std::ifstream ler_arquivo(entrada, std::ios::binary);
  std::ofstream escrever_arquivo(saida, std::ios::binary);

  uint8_t c;
  while ((c = ler_arquivo.get()) != unsigned_eof) {
    total_bytes++;
    contagem[c]++;
  }

  uint8_t found_byte = 0;

  uint32_t quantidade_bytes_diferentes = 0;
  for (int i = 0; i < 256; i++) {
    if (contagem[i] != 0) {
      found_byte = i;
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
        arvore_bytes[bytes_inseridos].caractere = i;

        heap.inserir({ bytes_inseridos, contagem[i] });

        bytes_inseridos++;
      }
    }

    while (heap.tamanho() > 1) {
      ElementoArvore dir = heap.extrairMin();
      ElementoArvore esq = heap.extrairMin();
      uint32_t peso_total = dir.peso + esq.peso;

      arvore_bytes[bytes_inseridos].conteudoNoInterno.dir = dir.indice;
      arvore_bytes[bytes_inseridos].conteudoNoInterno.esq = esq.indice;

      ElementoArvore no_interno = {
        bytes_inseridos, peso_total
      };

      heap.inserir(no_interno);

      bytes_inseridos++;
    }

    // voltando para o inicio do arquivo
    ler_arquivo.clear();
    ler_arquivo.seekg(std::ios_base::beg);

    void* void_nbytes_pointer = static_cast<void*>(&total_bytes);
    char* nbytes_pointer = static_cast<char*>(void_nbytes_pointer);
    escrever_arquivo.write(nbytes_pointer, sizeof(total_bytes));
    
    void* void_n_pointer = static_cast<void*>(&quantidade_bytes_diferentes);
    char* n_pointer = static_cast<char*>(void_n_pointer);
    escrever_arquivo.write(n_pointer, sizeof(quantidade_bytes_diferentes));

    void* void_tree_pointer = static_cast<void*>(arvore_bytes);
    char* tree_pointer = static_cast<char*>(void_tree_pointer);
    escrever_arquivo.write(tree_pointer, sizeof(No) * tamanho_vetor);
    
    TabelaValores tabela(arvore_bytes, bytes_inseridos);

    uint8_t byte_buf;
    uint8_t buf_ptr = 0;
    uint8_t buf_mask = 1;

    uint8_t c;
    while ((c = ler_arquivo.get()) != unsigned_eof) {
      InfoByte info = tabela.conteudo[c];

      for (uint64_t k = 0; k < info.qtd_bits; k++) {
        uint8_t bit_pos = k % 8;
        uint8_t byte_pos = k / 8;
        uint8_t bit_got = GET_BIN(info.bits_buff[byte_pos], bit_pos);
        
        if (bit_got) {
          byte_buf = SET_BIT_ON(byte_buf, buf_mask << buf_ptr);
        } else {
          byte_buf = SET_BIT_OFF(byte_buf, buf_mask << buf_ptr);
        }

        buf_ptr++;
        if (buf_ptr == 8) {
          escrever_arquivo << byte_buf;
          buf_ptr = 0;
        }
      }
    }

    if (buf_ptr != 0) {
      escrever_arquivo << byte_buf;
    }
    
    delete arvore_bytes;

    // CASO 2: número de bytes é igual a 1
  } else if (quantidade_bytes_diferentes == 1) {
    void* void_nbytes_pointer = static_cast<void*>(&total_bytes);
    char* nbytes_pointer = static_cast<char*>(void_nbytes_pointer);

    escrever_arquivo.write(nbytes_pointer, sizeof(total_bytes));
    escrever_arquivo << found_byte;

    // CASO 3: número de bytes é 0
  }
}

void Compactador::descompactar(const char* entrada, const char* saida) {
  std::ifstream ler_arquivo(entrada, std::ios::binary | std::ios::ate);
  std::ofstream escrever_arquivo(saida, std::ios::binary);
  uint32_t tamanho_arquivo = ler_arquivo.tellg();
  ler_arquivo.clear();
  ler_arquivo.seekg(std::ios::beg);
  
  if (tamanho_arquivo > 5) { // ARQUIVO COMPACTADO PELO CASO 1
    uint32_t total_bytes;
    uint32_t quantidade_bytes_diferentes;

    void* void_total_bytes_pointer = static_cast<void*>(&total_bytes);
    char* total_bytes_pointer = static_cast<char*>(void_total_bytes_pointer);
    ler_arquivo.read(total_bytes_pointer, sizeof(total_bytes));

    void* void_qtd_bytes_diff_pointer = static_cast<void*>(&quantidade_bytes_diferentes);
    char* qtd_bytes_diff_pointer = static_cast<char*>(void_qtd_bytes_diff_pointer);
    ler_arquivo.read(qtd_bytes_diff_pointer, sizeof(quantidade_bytes_diferentes));

    uint32_t tamanho_arvore = (quantidade_bytes_diferentes << 1) - 1;
    
    No* arvore = new No[tamanho_arvore];
    void* void_arvore_pointer = static_cast<void*>(arvore);
    char* arvore_pointer = static_cast<char*>(void_arvore_pointer);
    ler_arquivo.read(arvore_pointer, sizeof(No) * tamanho_arvore);
    uint32_t raiz = tamanho_arvore - 1;
    uint32_t folhas = (tamanho_arvore + 1) / 2;

    uint8_t byte_buff;
    uint8_t buff_ptr = 8;
    uint32_t bytes_lidos = 0;
    uint32_t idx_arvore = raiz;

    while (bytes_lidos < total_bytes) {
      while(idx_arvore >= folhas) {
        if (buff_ptr == 8) {
          buff_ptr = 0;
          byte_buff = ler_arquivo.get();
        }

        if (GET_BIN(byte_buff, buff_ptr)) {
          idx_arvore = arvore[idx_arvore].conteudoNoInterno.esq;
        } else {
          idx_arvore = arvore[idx_arvore].conteudoNoInterno.dir;
        }

        buff_ptr++;
      }

      uint8_t c = arvore[idx_arvore].caractere;
      escrever_arquivo << c;

      idx_arvore = raiz;
      bytes_lidos++;
    }
    
  } else if (tamanho_arquivo == 5) { // ARQUIVO COMPACTADO PELO CASO 2
    uint32_t total_bytes;

    void* void_total_bytes_pointer = static_cast<void*>(&total_bytes);
    char* total_bytes_pointer = static_cast<char*>(void_total_bytes_pointer);
    ler_arquivo.read(total_bytes_pointer, sizeof(total_bytes));

    uint8_t c = ler_arquivo.get();

    for (uint32_t i = 0; i < total_bytes; i++) {
      escrever_arquivo << c;
    }
  } // ARQUIVO COMPACTADO PELO CASO 3
}
