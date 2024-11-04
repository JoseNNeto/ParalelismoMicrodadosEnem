#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char col_b, col_c;

    while (fgets(linha, sizeof(linha), file)) {
         // Ignora a primeira linha se for cabeçalho
        if (linha_num == 0) {
            linha_num++;
            continue;
        }

        // Variáveis para armazenar os dados temporários
        char *token;
        int coluna = 0;

        char campo[256];
        int campo_pos = 0;
        token = strtok(linha, ",");
        for (int i = 0; i <= strlen(token); i++) {
            if (linha[i] == ';' || linha[i] == '\n' || linha[i] == '\0') {
                // Fecha o campo atual com NULL
                campo[campo_pos] = '\0';

                // Imprime o campo atual (inclusive os vazios)
                printf("  Coluna %d: %s\n", coluna, campo[0] == '\0' ? "(vazio)" : campo);

                // Reinicia para o próximo campo
                coluna++;
                campo_pos = 0;
                campo[0] = '\0';
            } else {
                // Adiciona o caractere ao campo atual
                campo[campo_pos++] = linha[i];
            }
        }

        // Separa a linha em colunas usando a vírgula como delimitador
        while (token != NULL) {
            // Verifica a coluna atual e salva as notas que queremos
            if (coluna == 31) {
                nota_cn = atof(token); // Coluna de Matemática
            }
            if (coluna == 32) {
                nota_mt = atof(token);   // Coluna de Ciências
            }
            if (coluna == 33) {
                nota_lc = atof(token);   // Coluna de Ciências
            }
            if (coluna == 34) {
                nota_ch = atof(token);   // Coluna de Ciências
            }
            if (coluna == 50) {
                nota_red = atof(token);  // Coluna de Ciências
            }
            // Avança para o próximo token (coluna)
            token = strtok(NULL, ";");
            coluna++;
            // printf("Linha: %d - Coluna %d: %s\n", linha_num, coluna, token);
        }

        // Exibe as notas que foram lidas para cada aluno
        // printf("Linha %d - Notas: CN %.2f, MT %.2f, LC %.2f, CH %.2f, RED %.2f\n", linha_num, nota_cn, nota_mt, nota_lc, nota_ch, nota_red);

        linha_num++;
    }

    fclose(file);
    return 0;
}
