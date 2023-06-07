#include <fstream>
#include <string>

#define SET_BIT_ON(value, mask) ((value) | (mask))
#define SET_BIT_OFF(value, mask) ((value) & ~(mask))
#define WRITE_BIN(dest, src, mask) (((src) & (mask)) | ((dest) & ~(mask)))
#define GET_BIN(src, pos) (((src) & (1 << (pos))) >> (pos))

struct ConteudoNoInterno {
  uint16_t esq;
  uint16_t dir;
};

union No {
  ConteudoNoInterno conteudoNoInterno;
  uint8_t caractere;
};

struct ElementoArvore {
  uint32_t indice;
  uint32_t peso;
};

bool operator < (const ElementoArvore& el1, const ElementoArvore& el2);

class Compactador {
  public:
    static void compactar(const char* entrada, const char* saida);
    static void descompactar(const char* entrada, const char* saida);
};

struct InfoByte {
  uint8_t qtd_bits;
  uint8_t bits_buff[32];
};

struct TabelaValores {
  InfoByte conteudo[256];
  TabelaValores(No* arvore_bytes, uint32_t tamanho);

  private:
    void percursoPreOrdem(No* arvore_bytes, uint16_t indice, uint32_t tamanho, uint32_t folhas, InfoByte info);
};
