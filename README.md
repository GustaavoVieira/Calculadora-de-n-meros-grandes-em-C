#  Calculadora de Números Grandes em C

Calculadora de precisão arbitrária implementada em C, capaz de realizar operações aritméticas com números inteiros e decimais de qualquer tamanho — sem as limitações dos tipos numéricos nativos da linguagem.

---

##  Motivação

Os tipos numéricos do C (`int`, `long`, `double`) têm um limite fixo de tamanho. Um `long long`, por exemplo, suporta apenas até ~9,2 × 10¹⁸. Já esta calculadora representa os números como **strings de caracteres** e implementa os algoritmos de cálculo manualmente, digito a digito, da mesma forma que fazemos no papel — sem limite de tamanho.

---

##  Funcionalidades

| Operação | Suporte |
|---|---|
| Soma | Inteiros e decimais, positivos e negativos |
| Subtração | Inteiros e decimais, positivos e negativos |
| Multiplicação | Inteiros e decimais, positivos e negativos |
| Exponenciação | Base livre, expoente inteiro ≥ 0 |
| Fatorial | Inteiros ≥ 0 |
| Expressão livre | Ex.: `2^10+5*3*2` com precedência de operadores |

---

##  Exemplo de uso

```
=== CALCULADORA DE NUMEROS GRANDES ===
1 - Soma
2 - Subtracao
3 - Multiplicacao
4 - Exponenciacao
5 - Fatorial
0 - Sair
Opcao: 5
Numero: 50
Resultado: 30414093201713378043612608166979581188299763898377856000000000000

Opcao: 4
Base: 2
Expoente: 100
Resultado: 1267650600228229401496703205376

Opcao: 3
A: 99999999999999999999
B: 99999999999999999999
Resultado: 9999999999999999999800000000000000000001

Digite uma expressao (ex: 2^10+5*3): 2^10+5*3
Resultado: 1039
```

---

##  Estrutura do código

```
calculadora.c
│
├── Funções auxiliares de string
│   ├── removeZeros()     — remove zeros desnecessários no início
│   ├── casasDecimais()   — conta dígitos após o ponto decimal
│   ├── removePonto()     — retira o ponto decimal da string
│   └── colocaPonto()     — reinsere o ponto na posição correta
│
├── Comparação
│   └── compara()         — compara dois inteiros positivos como string
│
├── Operações sobre inteiros (sem ponto/sinal)
│   ├── somaInteiros()         — soma dígito a dígito com carry
│   └── subtracaoInteiro()     — subtração dígito a dígito com borrow
│
├── Operações completas (com sinal e decimais)
│   ├── soma()            — trata sinais e alinhamento decimal
│   ├── subtracao()       — trata sinais e alinhamento decimal
│   ├── multiplicacao()   — multiplicação longa com produtos parciais
│   ├── exponenciacao()   — multiplicação repetida (a^e)
│   └── fatorial()        — produto acumulado de 1 até n
│
├── Avaliação de expressões
│   ├── precedencia()     — retorna a prioridade de um operador
│   ├── calcular()        — aplica o operador sobre dois operandos
│   └── expressao()       — Shunting-Yard com duas pilhas
│
└── main()                — menu interativo + entrada de expressão livre
```

---

##  Como funciona internamente

### Aritmética em string

Todos os números são tratados como `char*`. As operações replicam o que fazemos à mão:

- **Soma/Subtração:** percorre os dígitos da direita para a esquerda, gerenciando *carry* (vai-um) e *borrow* (empréstimo).
- **Multiplicação longa:** para cada dígito do segundo fator, calcula um produto parcial somando o primeiro fator repetidamente, aplica o deslocamento correto (zeros à direita) e acumula no resultado.
- **Decimais:** antes de operar, os pontos são removidos e as casas decimais são alinhadas com zeros. Ao final, o ponto é reinserido.

### Avaliação de expressões

Utiliza o algoritmo **Shunting-Yard** simplificado com duas pilhas: uma para números e outra para operadores. Ao encontrar um operador, resolve na pilha todos os operadores com precedência maior ou igual antes de empilhar o novo, garantindo a ordem correta de avaliação.

---

##  Limitações conhecidas

- Expressões **não suportam parênteses**.
- O **expoente** da exponenciação precisa ser um inteiro não-negativo.
- O **fatorial** aceita apenas inteiros não-negativos.
- A multiplicação usa somas repetidas por dígito, o que pode ser lento para números muito grandes com muitos dígitos decimais.
