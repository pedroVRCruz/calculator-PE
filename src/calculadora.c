/*
 ============================================================================
 Name        : calculadora.c
 Author      : Pedro, Cassia e João
 Version     :
 Description : Calculadora simples em C
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

void somar(int a, int b) {
    printf("A soma é: %d\n", a + b);
}

void subtrair(int a, int b) {
    printf("A subtração é: %d\n", a - b);
}

void multiplicar(int a, int b) {
    printf("A multiplicação é: %d\n", a * b);
}

void dividir(int a, int b) {
    if (b == 0) {
        printf("Erro: divisão por zero não é permitida.\n");
        return;
    }
    printf("A divisão é: %d\n", a / b);
}

void seed(int seed_value, int n) {
    srand(seed_value);

    printf("Número gerado: ");
    printf("%d", rand() % 9 + 1);

    for (int i = 1; i < n; i++) {
        printf("%d", rand() % 10);
    }
    printf("\n");
}

void soma_por_digitos(int a, int b) {
    long long a_long = (long long)a;
    long long b_long = (long long)b;
    int resultado[200];
    int i = 0;
    int sobra = 0;

    while (a_long > 0 || b_long > 0 || sobra > 0) {
        int d1 = a_long % 10;
        int d2 = b_long % 10;
        int soma = d1 + d2 + sobra;
        resultado[i++] = soma % 10;
        sobra = soma / 10;
        a_long /= 10;
        b_long /= 10;
    }

    printf("Resultado: ");
    for (int j = i - 1; j >= 0; j--) {
        printf("%d", resultado[j]);
    }
    printf("\n");
}

int main() {
    int num_usuario;
    int a, b;
    int continuar = 1;

    while (continuar) {
        printf("\nEscolha uma opção:\n");
        printf("1 - Soma\n");
        printf("2 - Subtração\n");
        printf("3 - Multiplicação\n");
        printf("4 - Divisão\n");
        printf("5 - Geração de número aleatório com seed\n");
        printf("6 - Soma dígito por dígito\n");
        printf("7 - Sair\n");
        printf("Opção: ");

        if (scanf("%d", &num_usuario) != 1) {
            // Leitura inválida: limpa buffer e continua
            printf("Entrada inválida.\n");
            // Consome caractere inválido
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (num_usuario) {
            case 1:
                printf("Digite dois números: \n");
                scanf("%d %d", &a, &b);
                somar(a, b);
                break;

            case 2:
                printf("Digite dois números: \n");
                scanf("%d %d", &a, &b);
                subtrair(a, b);
                break;

            case 3:
                printf("Digite dois números: \n");
                scanf("%d %d", &a, &b);
                multiplicar(a, b);
                break;

            case 4:
                printf("Digite dois números: \n");
                scanf("%d %d", &a, &b);
                dividir(a, b);
                break;

            case 5:
                printf("Digite o seed e a quantidade de dígitos: \n");
                scanf("%d %d", &a, &b);
                seed(a, b);
                break;

            case 6:
                printf("Digite dois números para somar dígito por dígito: \n");
                scanf("%d %d", &a, &b);
                soma_por_digitos(a, b);
                break;

            case 7:
                printf("Saindo do programa.\n");
                continuar = 0;
                break;

            default:
                printf("Opção inválida.\n");
                break;
        }
    }

    return 0;
}
