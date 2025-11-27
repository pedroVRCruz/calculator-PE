/*
 ============================================================================
 Name        : calculadora.c
 Author      : Pedro, Cassia e João
 Version     :
 Description : Calculadora simples em C com preparação para inteiros grandes
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Representa um inteiro arbitrariamente grande.
 * - sinal: +1 ou -1 (zero será tratado com sinal +1 e dígito único 0)
 * - n: quantidade de dígitos
 * - digitos: vetor de dígitos (base 10), onde digitos[0] é o menos significativo
 */
typedef struct {
    int sinal;
    int n;
    int *digitos;
} BigInt;

/*
 * Cria um BigInt a partir de uma string decimal, opcionalmente com sinal.
 * Exemplos de strings válidas: "123", "-45", "+0078".
 * Em caso de erro (caractere não numérico), retorna NULL.
 */
BigInt *big_criar(const char *texto) {
    if (texto == NULL) {
        return NULL;
    }

    /* Pular espaços em branco iniciais */
    while (*texto == ' ' || *texto == '\t' || *texto == '\n' || *texto == '\r') {
        texto++;
    }

    int sinal = 1;
    int i = 0;

    /* Tratar sinal explícito, se existir */
    if (texto[i] == '-') {
        sinal = -1;
        i++;
    } else if (texto[i] == '+') {
        sinal = 1;
        i++;
    }

    /* Pular zeros à esquerda para evitar armazenar dígitos desnecessários */
    while (texto[i] == '0') {
        i++;
    }

    int inicio_digitos = i;
    int len = (int)strlen(texto);

    /* Caso especial: número é zero (tudo era sinal+zeros+fim de string) */
    if (inicio_digitos >= len) {
        BigInt *z = (BigInt *)malloc(sizeof(BigInt));
        if (!z) return NULL;

        z->sinal = 1;
        z->n = 1;
        z->digitos = (int *)malloc(sizeof(int));
        if (!z->digitos) {
            free(z);
            return NULL;
        }
        z->digitos[0] = 0;
        return z;
    }

    int qtd_digitos = len - inicio_digitos;

    BigInt *num = (BigInt *)malloc(sizeof(BigInt));
    if (!num) {
        return NULL;
    }

    num->sinal = sinal;
    num->n = qtd_digitos;
    num->digitos = (int *)malloc(qtd_digitos * sizeof(int));
    if (!num->digitos) {
        free(num);
        return NULL;
    }

    /*
     * Preenche o vetor de dígitos do menos significativo para o mais significativo.
     * texto[inicio_digitos] é o dígito mais à esquerda (mais significativo).
     * Aqui começamos do final da string (len - 1) e vamos voltando.
     */
    int j_texto = len - 1;
    for (int j = 0; j < qtd_digitos; j++, j_texto--) {
        char c = texto[j_texto];
        if (c < '0' || c > '9') {
            /* Caractere inválido: libera memória e retorna NULL */
            free(num->digitos);
            free(num);
            return NULL;
        }
        num->digitos[j] = c - '0';
    }

    return num;
}

/* Libera a memória associada a um BigInt */
void big_destruir(BigInt *x) {
    if (x == NULL) return;
    if (x->digitos != NULL) {
        free(x->digitos);
    }
    free(x);
}

/*
 * Imprime um BigInt em stdout, respeitando o sinal.
 * Por construção, zero é armazenado como n=1, digitos[0]=0 e sinal=+1.
 */
void big_imprimir(const BigInt *x) {
    if (x == NULL) {
        printf("(null)");
        return;
    }

    int eh_zero = (x->n == 1 && x->digitos[0] == 0);

    if (x->sinal < 0 && !eh_zero) {
        printf("-");
    }

    /* Imprime do dígito mais significativo para o menos significativo */
    for (int i = x->n - 1; i >= 0; i--) {
        printf("%d", x->digitos[i]);
    }
}

/* -------------------------------------------------------------------------
 * Funções da calculadora simples (versão com int / long long)
 * ------------------------------------------------------------------------- */

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

/* Gera um número pseudoaleatório com quantidade de dígitos controlada por n */
void seed(int seed_value, int n) {
    srand(seed_value);

    printf("Número gerado: ");
    /* Primeiro dígito não pode ser zero */
    printf("%d", rand() % 9 + 1);

    for (int i = 1; i < n; i++) {
        printf("%d", rand() % 10);
    }
    printf("\n");
}

/*
 * Soma dígito por dígito de dois inteiros comuns, simulando soma de "números grandes".
 * Ainda está usando int/long long, mas a lógica serve como base para BigInt.
 */
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
            /* Leitura inválida: limpa buffer e continua */
            printf("Entrada inválida.\n");
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
