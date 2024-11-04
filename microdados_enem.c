#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <mpi.h>

#define FILENAME "./microdados_enem_2023/DADOS/MICRODADOS_ENEM_2023.csv"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *file;
    int total_inscritos = 0;
    int linha_num = 1;

    // Cada processo lê seu próprio arquivo e faz cálculos parciais
    float maior_nota = FLT_MIN;
    float menor_nota = FLT_MAX;
    float soma_notas = 0;
    int provas_validas = 0;

    // Dividir o trabalho: cada processo lê partes do arquivo
    if (rank == 0) {
        file = fopen(FILENAME, "r");
        if (file == NULL) {
            printf("Erro ao abrir o arquivo\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Contar o número total de linhas (inscritos)
        char linha[1024];
        while (fgets(linha, sizeof(linha), file)) {
            total_inscritos++;
        }
        rewind(file); // Volta ao início do arquivo para leitura

        // Enviar o total de inscritos para todos os processos
        MPI_Bcast(&total_inscritos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        // Receber o total de inscritos
        MPI_Bcast(&total_inscritos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Calcular a quantidade de linhas que cada processo deve ler
    int lines_per_process = total_inscritos / size;
    int start_line = rank * lines_per_process;
    int end_line = (rank == size - 1) ? total_inscritos : start_line + lines_per_process;

    // Ler as linhas atribuídas a cada processo
    file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char linha[1024];
    while (fgets(linha, sizeof(linha), file)) {
        if (linha_num == 1) { // Ignorar cabeçalho
            linha_num++;
            continue;
        }

        // Processar apenas as linhas atribuídas a este processo
        if (linha_num > start_line && linha_num <= end_line) {
            float nota_cn, nota_mt, nota_lc, nota_ch, nota_red;
            int coluna = 0;
            char campo[256];
            int campo_pos = 0;

            // Reseta as notas para cada linha
            nota_cn = nota_mt = nota_lc = nota_ch = nota_red = 0;

            for (int i = 0; i <= strlen(linha); i++) {
                if (linha[i] == ';' || linha[i] == '\n' || linha[i] == '\0') {
                    campo[campo_pos] = '\0';
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

                    coluna++;
                    campo_pos = 0;
                } else {
                    campo[campo_pos++] = linha[i];
                }
            }

            if (nota_cn > 0 && nota_mt > 0 && nota_lc > 0 && nota_ch > 0 && nota_red > 0) {
                float nota_total = nota_cn + nota_mt + nota_lc + nota_ch + nota_red;

                if (nota_total > maior_nota) {
                    maior_nota = nota_total;
                }
                if (nota_total < menor_nota) {
                    menor_nota = nota_total;
                }

                soma_notas += nota_total;
                provas_validas++;
            }
        }
        linha_num++;
    }

    fclose(file);

    // Reduzir os resultados para o processo 0
    float global_maior_nota, global_menor_nota, global_soma_notas;
    int global_provas_validas;

    MPI_Reduce(&maior_nota, &global_maior_nota, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&menor_nota, &global_menor_nota, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&soma_notas, &global_soma_notas, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&provas_validas, &global_provas_validas, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Apenas o processo 0 exibe os resultados
    if (rank == 0) {
        float media_nota = (global_provas_validas > 0) ? (global_soma_notas / global_provas_validas) : 0;
        printf("Maior Nota: %.2f\n", global_maior_nota / 5);
        printf("Menor Nota: %.2f\n", global_menor_nota / 5);
        printf("Média das Notas: %.2f\n", media_nota / 5);
        printf("Total de Provas Válidas: %d\n", global_provas_validas);
        printf("Total de Inscritos: %d\n", total_inscritos);
    }

    MPI_Finalize();
    return 0;
}
