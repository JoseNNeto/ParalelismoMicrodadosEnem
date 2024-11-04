#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <mpi.h>

#define FILENAME "./microdados_enem_2023/DADOS/MICRODADOS_ENEM_2023.csv"

int main() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }

    char linha[1024];
    int linha_num = 1;
    float nota_cn, nota_mt, nota_lc, nota_ch, nota_red;
    
    // Variáveis para cálculo dos dados principais
    float maior_nota = FLT_MIN;
    float menor_nota = FLT_MAX;
    float soma_notas = 0;
    int provas_validas = 0;
    int total_inscritos = 0;

    // Lê cada linha do arquivo
    while (fgets(linha, sizeof(linha), file)) {
        // Ignora a primeira linha (cabeçalho)
        if (linha_num == 1) {
            linha_num++;
            continue;
        }

        // Variáveis de controle
        int coluna = 0;
        char campo[256];
        int campo_pos = 0;

        // Reseta as notas para cada linha
        nota_cn = nota_mt = nota_lc = nota_ch = nota_red = 0;

        // Percorre cada caractere da linha para construir campos
        for (int i = 0; i <= strlen(linha); i++) {
            if (linha[i] == ';' || linha[i] == '\n' || linha[i] == '\0') {
                campo[campo_pos] = '\0';

                // Verifica se é uma das colunas de interesse e converte o valor
                if (coluna == 31) {
                    nota_cn = atof(campo);
                } else if (coluna == 32) {
                    nota_mt = atof(campo);
                } else if (coluna == 33) {
                    nota_lc = atof(campo);
                } else if (coluna == 34) {
                    nota_ch = atof(campo);
                } else if (coluna == 50) {
                    nota_red = atof(campo);
                }

                // Reinicia para o próximo campo
                coluna++;
                campo_pos = 0;
            } else {
                campo[campo_pos++] = linha[i];
            }
        }

        // Verifica se as notas são válidas para contagem e cálculo
        if (nota_cn > 0 && nota_mt > 0 && nota_lc > 0 && nota_ch > 0 && nota_red > 0) {
            float nota_total = nota_cn + nota_mt + nota_lc + nota_ch + nota_red;

            // Atualiza o maior e menor nota
            if (nota_total > maior_nota) {
                maior_nota = nota_total;
            }
            if (nota_total < menor_nota) {
                menor_nota = nota_total;
            }

            // Soma a nota total para cálculo da média
            soma_notas += nota_total;
            provas_validas++;
        }

        total_inscritos++;
        linha_num++;
    }

    fclose(file);

    // Calcula a média
    float media_nota = (provas_validas > 0) ? (soma_notas / provas_validas) : 0;

    // Exibe os resultados
    printf("Maior Nota: %.2f\n", maior_nota/5);
    printf("Menor Nota: %.2f\n", menor_nota/5);
    printf("Média das Notas: %.2f\n", media_nota/5);
    printf("Total de Provas Válidas: %d\n", provas_validas);
    printf("Total de Inscritos: %d\n", total_inscritos);

    return 0;
}
