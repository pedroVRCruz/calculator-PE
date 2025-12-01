/*
 ============================================================================
 Name        : calculadora.c
 Author      : Pedro, Cassia e João
 Version     :
 Description : Calculadora em C
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

/* -------------------------------------------------------------------------
 * Funções utilitárias para BigInt
 * ------------------------------------------------------------------------- */

/* Cria um BigInt representando zero, com espaço para "tamanho" dígitos. */
BigInt *big_criar_zero(int tamanho) {
    if (tamanho < 1) tamanho = 1;

    BigInt *z = (BigInt *)malloc(sizeof(BigInt));
    if (!z) return NULL;

    z->sinal = 1;
    z->n = 1;
    z->digitos = (int *)calloc(tamanho, sizeof(int));
    if (!z->digitos) {
        free(z);
        return NULL;
    }

    z->digitos[0] = 0;
    return z;
}

/* Remove zeros à esquerda e garante que zero tenha sinal positivo. */
void big_normalizar(BigInt *x) {
    if (!x) return;

    while (x->n > 1 && x->digitos[x->n - 1] == 0) {
        x->n--;
    }

    if (x->n == 1 && x->digitos[0] == 0) {
        x->sinal = 1;
    }
}

/* Cria uma cópia profunda de um BigInt. */
BigInt *big_copiar(const BigInt *orig) {
    if (!orig) return NULL;

    BigInt *copia = (BigInt *)malloc(sizeof(BigInt));
    if (!copia) return NULL;

    copia->sinal = orig->sinal;
    copia->n = orig->n;
    copia->digitos = (int *)malloc(orig->n * sizeof(int));
    if (!copia->digitos) {
        free(copia);
        return NULL;
    }

    for (int i = 0; i < orig->n; i++) {
        copia->digitos[i] = orig->digitos[i];
    }

    return copia;
}

/* Compara apenas os módulos. Retorna -1 se a < b, 0 se igual, 1 se a > b. */
int big_comparar_abs(const BigInt *a, const BigInt *b) {
    if (a->n != b->n) {
        return (a->n < b->n) ? -1 : 1;
    }

    for (int i = a->n - 1; i >= 0; i--) {
        if (a->digitos[i] != b->digitos[i]) {
            return (a->digitos[i] < b->digitos[i]) ? -1 : 1;
        }
    }
    return 0;
}

/* Subtrai módulos assumindo a >= b e ambos não negativos. */
BigInt *big_subtrair_abs(const BigInt *a, const BigInt *b) {
    BigInt *resultado = big_criar_zero(a->n);
    if (!resultado) return NULL;

    resultado->n = a->n;

    int emprestimo = 0;
    for (int i = 0; i < a->n; i++) {
        int valor_a = a->digitos[i];
        int valor_b = (i < b->n) ? b->digitos[i] : 0;

        int sub = valor_a - valor_b - emprestimo;
        if (sub < 0) {
            sub += 10;
            emprestimo = 1;
        } else {
            emprestimo = 0;
        }
        resultado->digitos[i] = sub;
    }

    big_normalizar(resultado);
    return resultado;
}

/* Multiplica o número atual por 10 e adiciona um dígito (0-9). */
int big_multiplicar_por10_adicionar(BigInt *x, int digito) {
    if (!x || digito < 0 || digito > 9) return -1;

    int *novo = (int *)realloc(x->digitos, (x->n + 1) * sizeof(int));
    if (!novo) return -1;
    x->digitos = novo;

    /* desloca os dígitos para abrir espaço para o novo menos significativo */
    for (int i = x->n - 1; i >= 0; i--) {
        x->digitos[i + 1] = x->digitos[i];
    }
    x->digitos[0] = digito;
    x->n += 1;

    big_normalizar(x);
    return 0;
}

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

/*funcao de somar inteiros */
BigInt* big_somar(const BigInt *a, const BigInt *b) {
    
    int auxiliar = 0;
    int i = 0;

    /*variavel que verifica qual dos valores é maior para o loop*/
    int maximovalor = (a->n > b->n) ? a->n : b->n;

    /*set de memoria do resultado*/
    BigInt *respostafinalsoma = (BigInt *)malloc(sizeof(BigInt));
    
    /*set espaço maior*/
    respostafinalsoma->digitos = (int *)malloc((maximovalor * 2) * sizeof(int));

    /*loop que percorre os vetores*/
    for (i = 0; i < maximovalor; i++) {
        /*completa as informações faltantes com 0 para somar os vetores em a */
        int valor_a = (i < a->n) ? a->digitos[i] : 0; 

       /*mesma logica para b*/
        int valor_b = (i < b->n) ? b->digitos[i] : 0;

        /*soma as colunas mais o auxiliar*/
        int soma = valor_a + valor_b + auxiliar;
        respostafinalsoma->digitos[i] = soma % 10;

        /*utiliza o auxiliar */
        auxiliar = soma / 10;
    }

    /*considera se tem algo no auxiliar salva ele na soma*/
    if (auxiliar > 0) {
        respostafinalsoma->digitos[i] = auxiliar;
        respostafinalsoma->n = maximovalor + 1; 
    } else {
        respostafinalsoma->n = maximovalor;
    }

    return respostafinalsoma;
}

BigInt* big_subtrair(const BigInt *a, const BigInt *b) {
    int auxiliar = 0;
    int i = 0;

    /*variavel que verifica qual dos valores é maior para o loop*/
    int maximovalor = (a->n > b->n) ? a->n : b->n;

    /*set de memoria do resultado*/
    BigInt *respostafinalsubtrair = (BigInt *)malloc(sizeof(BigInt));
    
    /*set espaço maior*/
    respostafinalsubtrair->digitos = (int *)malloc((maximovalor * 2) * sizeof(int));

    /*loop que percorre os vetores*/
    for (i = 0; i < maximovalor; i++) {
        /*completa as informações faltantes com 0 para somar os vetores em a */
        int valor_a = (i < a->n) ? a->digitos[i] : 0; 

       /*mesma logica para b*/
        int valor_b = (i < b->n) ? b->digitos[i] : 0;

        /*soma as colunas mais o auxiliar*/
        int subtracao = valor_a - valor_b - auxiliar;
        respostafinalsubtrair->digitos[i] = subtracao % 10;

        /*utiliza o auxiliar */
        auxiliar = subtracao / 10;
    }

    /*considera se tem algo no auxiliar salva ele na soma*/
    if (auxiliar > 0) {
        respostafinalsubtrair->digitos[i] = auxiliar;
        respostafinalsubtrair->n = maximovalor + 1; 
    } else {
        respostafinalsubtrair->n = maximovalor;
    }

    return respostafinalsubtrair;
}

/* Divide dois BigInt e retorna quociente; resto opcionalmente é retornado em resto_out. */
BigInt *big_dividir_mod(const BigInt *dividendo, const BigInt *divisor, BigInt **resto_out) {
    if (!dividendo || !divisor) return NULL;

    /* Verificação de divisão por zero */
    if (divisor->n == 1 && divisor->digitos[0] == 0) {
        printf("Erro: divisão por zero não é permitida.\n");
        return NULL;
    }

    BigInt *dividendo_abs = big_copiar(dividendo);
    BigInt *divisor_abs = big_copiar(divisor);
    if (!dividendo_abs || !divisor_abs) {
        big_destruir(dividendo_abs);
        big_destruir(divisor_abs);
        return NULL;
    }
    dividendo_abs->sinal = 1;
    divisor_abs->sinal = 1;

    /* Pré-aloca quociente e resto com espaço suficiente. */
    BigInt *quociente = big_criar_zero(dividendo_abs->n);
    BigInt *resto = big_criar_zero(dividendo_abs->n + 1);
    if (!quociente || !resto) {
        big_destruir(dividendo_abs);
        big_destruir(divisor_abs);
        big_destruir(quociente);
        big_destruir(resto);
        return NULL;
    }

    quociente->n = dividendo_abs->n;

    /* Algoritmo de divisão longa: percorre dos dígitos mais significativos para os menos. */
    for (int i = dividendo_abs->n - 1; i >= 0; i--) {
        if (big_multiplicar_por10_adicionar(resto, dividendo_abs->digitos[i]) != 0) {
            big_destruir(dividendo_abs);
            big_destruir(divisor_abs);
            big_destruir(quociente);
            big_destruir(resto);
            return NULL;
        }

        int q_digit = 0;
        while (big_comparar_abs(resto, divisor_abs) >= 0) {
            BigInt *novo_resto = big_subtrair_abs(resto, divisor_abs);
            if (!novo_resto) {
                big_destruir(dividendo_abs);
                big_destruir(divisor_abs);
                big_destruir(quociente);
                big_destruir(resto);
                return NULL;
            }
            big_destruir(resto);
            resto = novo_resto;
            q_digit++;
        }

        quociente->digitos[i] = q_digit;
    }

    big_normalizar(quociente);

    /* Define sinais de acordo com as regras matemáticas. */
    if (quociente->n == 1 && quociente->digitos[0] == 0) {
        quociente->sinal = 1;
    } else {
        quociente->sinal = dividendo->sinal * divisor->sinal;
    }

    if (resto->n == 1 && resto->digitos[0] == 0) {
        resto->sinal = 1;
    } else {
        resto->sinal = dividendo->sinal;
    }

    big_destruir(dividendo_abs);
    big_destruir(divisor_abs);

    if (resto_out) {
        *resto_out = resto;
    } else {
        big_destruir(resto);
    }

    return quociente;
}

/* Retorna apenas o quociente da divisão. */
BigInt *big_dividir(const BigInt *dividendo, const BigInt *divisor) {
    return big_dividir_mod(dividendo, divisor, NULL);
}

/* Retorna apenas o resto (módulo) da divisão. */
BigInt *big_mod(const BigInt *dividendo, const BigInt *divisor) {
    BigInt *resto = NULL;
    BigInt *quociente = big_dividir_mod(dividendo, divisor, &resto);
    big_destruir(quociente);
    return resto;
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

/*Implementa um menu para operações int e long long*/
void menu_inteiros_entrada_usuario() {
    int num_usuario;
    int a, b;
    int continuar = 1;

    while (continuar) {
        printf("\n============================================================\n");
        printf("                          CALCULADORA INT                    \n");
        printf("============================================================\n");
        printf("  [1] ➜ Soma\n");
        printf("  [2] ➜ Subtração\n");
        printf("  [3] ➜ Multiplicação\n");
        printf("  [4] ➜ Divisão\n");
        printf("  [5] ➜ Geração de número aleatório com seed\n");
        printf("  [6] ➜ Soma dígito por dígito\n");
        printf("------------------------------------------------------------\n");
        printf("  [7] ➜ Voltar ao menu principal\n");
        printf("============================================================\n");
        printf("Escolha uma opção: ");

        if (scanf("%d", &num_usuario) != 1) {
            printf("Entrada inválida.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (num_usuario) {
            case 1:
                printf("Digite dois números: ");
                scanf("%d %d", &a, &b);
                somar(a, b);
                break;

            case 2:
                printf("Digite dois números: ");
                scanf("%d %d", &a, &b);
                subtrair(a, b);
                break;

            case 3:
                printf("Digite dois números: ");
                scanf("%d %d", &a, &b);
                multiplicar(a, b);
                break;

            case 4:
                printf("Digite dois números: ");
                scanf("%d %d", &a, &b);
                dividir(a, b);
                break;

            case 5:
                printf("Digite o seed e a quantidade de dígitos: ");
                scanf("%d %d", &a, &b);
                seed(a, b);
                break;

            case 6:
                printf("Digite dois números: ");
                scanf("%d %d", &a, &b);
                soma_por_digitos(a, b);
                break;

            case 7:
                return;

            default:
                printf("Opção inválida.\n");
                break;
        }
    }
}

/*Implementa um menu para operações com bigint*/
void menu_bigint_entrada_usuario() {
    int opc = 1;
    char A[1024], B[1024];

    while (opc) {
    printf("\n============================================================\n");
    printf("                      CALCULADORA BIGINT                   \n");
    printf("============================================================\n");
    printf("  [1] ➜ Soma\n");
    printf("  [2] ➜ Subtração\n");
    printf("  [3] ➜ Multiplicação\n");
    printf("  [4] ➜ Divisão\n");
    printf("  [5] ➜ Módulo\n");
    printf("------------------------------------------------------------\n");
    printf("  [6] ➜ Voltar ao menu principal\n");
    printf("============================================================\n");
    printf("Escolha uma opção: ");
        scanf("%d", &opc);

        if (opc == 6) break;

        if (opc < 1 || opc > 6) {
            printf("Opção inválida.\n");
            continue;
        }

        printf("Digite o primeiro número: ");
        scanf("%s", A);

        printf("Digite o segundo número: ");
        scanf("%s", B);

        BigInt *a = big_criar(A);
        BigInt *b = big_criar(B);
        BigInt *r = NULL;

        switch (opc) {
            case 1:
                r = big_somar(a, b);
                printf("Soma: ");
                big_imprimir(r);
                printf("\n");
                break;
            case 2:
                r = big_subtrair(a, b);
                printf("Subtração: ");
                big_imprimir(r);
                printf("\n");
                break;
            case 3:
                printf("Multiplicação ainda não implementada.\n");
                break;
            case 4:
                r = big_dividir(a, b);
                if (r) {
                    printf("Quociente: ");
                    big_imprimir(r);
                    printf("\n");
                }
                break;
            case 5: {
                BigInt *resto = big_mod(a, b);
                if (resto) {
                    printf("Resto: ");
                    big_imprimir(resto);
                    printf("\n");
                }
                r = resto;
                break;
            }

            default:
                printf("Opção inválida.\n");
        }

        big_destruir(a);
        big_destruir(b);
        big_destruir(r);
    }
}


/* ------------------------------
        MAIN PRINCIPAL
--------------------------------- */
int main() {
    int opc = 1;

    while (opc) {
        printf("\n============================================================\n");
        printf("                        MENU PRINCIPAL                      \n");
        printf("============================================================\n");
        printf("  [1] ➜ Calculadora Int\n");
        printf("  [2] ➜ Calculadora BigInt\n");
        printf("------------------------------------------------------------\n");
        printf("  [3] ➜ Sair\n");
        printf("============================================================\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opc);

        switch (opc) {

            case 1: {
                printf("\n============================================================\n");
                printf("                       TIPO DA ENTRADA                     \n");
                printf("============================================================\n");
                printf("  [1] ➜ Entrada do usuário\n");
                printf("  [2] ➜ Entrada por arquivo\n");
                printf("============================================================\n");
                printf("Escolha uma opção: ");
                
                int tipo_entrada;
                scanf("%d", &tipo_entrada);

                switch (tipo_entrada) {
                    case 1:
                        printf("Entrada de usuário selecionada.\n");
                        menu_inteiros_entrada_usuario();
                        break;

                    case 2:
                        printf("Entrada de arquivo selecionada.\n");
                        // menu_inteiros_entrada_txt();
                        break;

                    default:
                        printf("Opção inválida. Usando entrada de usuário por padrão.\n");
                        menu_inteiros_entrada_usuario();
                }
                break;
            }

            case 2: {
                printf("\n============================================================\n");
                printf("                       TIPO DA ENTRADA                     \n");
                printf("============================================================\n");
                printf("  [1] ➜ Entrada do usuário\n");
                printf("  [2] ➜ Entrada por arquivo\n");
                printf("============================================================\n");
                printf("Escolha uma opção: ");
                
                int tipo_entrada_bigint;

                scanf("%d", &tipo_entrada_bigint);
                switch (tipo_entrada_bigint) {
                    case 1:
                        printf("Entrada de usuário selecionada.\n");
                        menu_bigint_entrada_usuario();
                        break;

                    case 2:
                        printf("Entrada de arquivo selecionada.\n");
                        // menu_bigint_entrada_txt();
                        break;

                    default:
                        printf("Opção inválida. Usando entrada de usuário por padrão.\n");
                }
                break;
            }

            case 3:
                printf("Saindo...\n");
                return 0;

            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}
