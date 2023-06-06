#include "./compactador/compactador.h"

int main() {
    Compactador compactador;
    compactador.compactar("teste.txt", "saida");
    compactador.descompactar("saida", "saida.txt");
    return 0;
}