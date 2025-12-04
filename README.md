# Calculadora BigInt & Int em C

Documentação da criação de uma calculadora, capaz de realizar operações aritméticas com números inteiros grandes (**BigInt**). O projeto também inclui um modo para inteiros padrão.

> **Autores:** Pedro, Cassia e João  
> **Versão:** 1.0 

---

## Índice
1. [Funcionalidades](#-funcionalidades)
2. [Estrutura de Dados](#-estrutura-de-dados-bigint)
4. [Como Usar](#-como-usar)
5. [Automação (Entrada por Arquivo)](#-automação-entrada-por-arquivo)
6. [Documentação das Funções](#-documentação-técnica)

---

## Funcionalidades

### 🔹 Modo BigInt (Números Grandes)
Projetado para contornar o *overflow* de tipos primitivos (`int`, `long long`).
* **Operações Suportadas:** Soma, Subtração, Multiplicação, Divisão,Módulo (Resto) e uma operação personalizada.
* **Capacidade:** Suporta números com centenas ou milhares de dígitos.
* **Sinais:** Manipulação correta de números positivos e negativos.

### 🔹 Modo Int (Padrão)
Para cálculos rápidos e didática.
* Operações básicas (+, -, *, /).
* **Gerador de Aleatórios:** Criação de números baseada em *seed*.
* **Didático:** Demonstração visual de soma "dígito por dígito".

---

## Estrutura de Dados (BigInt)

O núcleo do projeto é a `struct BigInt`, que armazena números como vetores dinâmicos, onde cada posição do vetor representa um dígito decimal.

```c
typedef struct {
    int sinal;      // +1 ou -1
    int n;          // Quantidade de dígitos
    int *digitos;   // Vetor dinâmico
} BigInt;
```

## Como usar

Ao executar o programa, você será levado ao **Menu Principal**, onde poderá escolher entre:

1. **Calculadora Int (padrão)**
2. **Calculadora BigInt**
3. **Sair**

---

### 1. Modo Int (calculadora comum)

Permite operações com inteiros tradicionais do C.

**Operações disponíveis:**
- Soma (`a + b`)
- Subtração (`a - b`)
- Multiplicação (`a * b`)
- Divisão (`a / b`)
- Geração de número aleatório com seed
- Soma por dígitos (demonstração tipo BigInt)

**Fluxo:**
1. Escolha a operação.
2. Digite os valores solicitados.
3. Veja o resultado imediatamente.

---

### 2. Modo BigInt (números gigantes)

Neste modo você pode realizar operações com números de **qualquer tamanho**, como:

- `99999999999999999999999 + 88888888888888888888888`
- `12345678901234567890 * 98765432109876543210`

**Operações disponíveis:**
- Soma
- Subtração
- Multiplicação
- Divisão
- Módulo (resto)

**Fluxo:**
1. Selecione o tipo de entrada:
   - **1. Entrada manual**
   - **2. Entrada por arquivo**
2. Insira ou carregue dois valores BigInt.
3. Escolha a operação desejada.
4. O resultado será exibido completo, independente do tamanho.

---
