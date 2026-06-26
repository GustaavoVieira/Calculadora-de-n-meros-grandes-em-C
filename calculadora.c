/*
 * Calculadora de Numeros Grandes em C
 * ------------------------------------
 * Bibliotecas utilizadas:
 *   stdio.h  - printf e scanf (entrada e saida no terminal)
 *   stdlib.h - malloc, realloc, free (gerenciamento de memoria) e atoi (string -> int)
 *   string.h - strlen, strcmp, strcpy, memmove, strcat, strncpy, strchr
 *   ctype.h  - isdigit (verifica se um caractere e um digito de 0 a 9)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* 
  FUNCOES AUXILIARES DE MANIPULACAO DE STRING
*/
void removeZeros(char *s) {
    int i = 0;

    /* Avanca enquanto o caractere atual for '0' E ainda existir
      pelo menos um caractere depois dele (evita apagar o "0" isolado). */
    while (s[i] == '0' && s[i+1] != '\0')
        i++;
    /* Se encontrou zeros para remover, desloca toda a string para a
      esquerda usando memmove (seguro mesmo com areas sobrepostas).
      O +1 no tamanho garante que o '\0' final tambem seja copiado. */
    if (i > 0)
        memmove(s, s + i, strlen(s) - i + 1);
}
/*
  casasDecimais: retorna quantos digitos existem apos o ponto decimal.
 */
int casasDecimais(char *s) {
    /* Procura o primeiro '.' na string. */
    char *p = strchr(s, '.');
    /* Se nao houver ponto, o numero e inteiro: 0 casas decimais. */
    if (p == NULL)
        return 0;
    /* p aponta para o '.'; p+1 aponta para o primeiro digito decimal.
      strlen(p+1) conta quantos caracteres ha apos o ponto. */
    return strlen(p + 1);
}

/*
  removePonto: retira o ponto decimal da string, transformando o
  numero em um inteiro.
*/
void removePonto(char *s) {
    char *p = strchr(s, '.');
    /* Se houver ponto, sobrescreve-o deslocando tudo que vem depois
      uma posicao para a esquerda (incluindo o '\0'). */
    if (p != NULL)
        memmove(p, p + 1, strlen(p));
}
/*
  colocaPonto: insere o ponto decimal na posicao correta de um numero
  inteiro (representado como string), de acordo com a quantidade de
  casas decimais desejadas.
 */
char *colocaPonto(char *s, int casas) {

    /* Se nao ha casas decimais, nao ha nada a fazer. */
    if (casas == 0)
        return s;

    int tam = strlen(s);

    /* Caso especial: o numero de digitos e menor ou igual ao numero de
      casas decimais, logo o resultado e menor que 1  */
    if (tam <= casas) {
        /* Precisamos de: "0." + zeros de preenchimento + digitos + '\0'*/
        char *r = malloc(casas + 3);
        r[0] = '0';
        r[1] = '.';
        /* Calcula quantos zeros de preenchimento sao necessarios entre
          o ponto e os digitos significativos. */
        int zeros = casas - tam;

        for (int i = 0; i < zeros; i++)
            r[i + 2] = '0';

        /* Copia os digitos do numero apos os zeros de preenchimento. */
        strcpy(r + zeros + 2, s);

        free(s);
        return r;
    }

    /* Caso normal: separa a parte inteira da parte decimal com um ponto. */
    char *r = malloc(tam + 2); /* +2: espaco para o '.' e o '\0' */

    /* Copia os digitos da parte inteira (os primeiros tam-casas digitos). */
    strncpy(r, s, tam - casas);

    /* Insere o ponto decimal na posicao correta. */
    r[tam - casas] = '.';

    /* Copia os digitos da parte decimal (os ultimos 'casas' digitos). */
    strcpy(r + tam - casas + 1, s + tam - casas);

    free(s);
    return r;
}


/*  COMPARACAO DE NUMEROS INTEIROS POSITIVOS (SEM SINAL, SEM PONTO) */

/*
  compara: compara dois numeros inteiros positivos representados como strings.
  Retorna:
    1  se a > b
   -1  se a < b
    0  se a == b
 
  Estrategia: primeiro compara pelo numero de digitos (mais digitos = maior).
  Se tiverem o mesmo tamanho, compara lexicograficamente (funciona porque
  os digitos estao em ordem crescente de '0' a '9' na tabela ASCII).
 */
int compara(char *a, char *b) {

    /* Cria copias para nao alterar as strings originais, pois
      removeZeros modifica a string diretamente. */
    char *aa = malloc(strlen(a) + 1);
    char *bb = malloc(strlen(b) + 1);

    strcpy(aa, a);
    strcpy(bb, b);

    removeZeros(aa);
    removeZeros(bb);

    int ta = strlen(aa);
    int tb = strlen(bb);

    int resp;

    if (ta > tb)
        resp = 1;        /* aa tem mais digitos, logo e maior. */
    else if (ta < tb)
        resp = -1;       /* aa tem menos digitos, logo e menor. */
    else {
        /* Mesmo numero de digitos: compara como texto (funciona para inteiros). */
        int c = strcmp(aa, bb);
        if      (c > 0) resp =  1;
        else if (c < 0) resp = -1;
        else            resp =  0;
    }

    free(aa);
    free(bb);

    return resp;
}


/*OPERACOES SOBRE INTEIROS (SEM PONTO DECIMAL, SEM SINAL NEGATIVO) */

/*
  somaInteiros: soma dois numeros inteiros positivos representados como
  strings e retorna o resultado em uma nova string alocada dinamicamente.
 
  Algoritmo: replica a soma "de baixo para cima" que fazemos no papel.
    - Percorre os dois numeros da direita para a esquerda (do menor para
     o maior digito).
    - Soma os digitos correspondentes mais o "vai-um" (carry).
    - Guarda os digitos do resultado em ordem invertida num vetor auxiliar.
    - Ao final, inverte o vetor para obter o resultado na ordem correta.
 */
char *somaInteiros(char *a, char *b) {
    int i = strlen(a) - 1; /* Indice do ultimo digito de a. */
    int j = strlen(b) - 1; /* Indice do ultimo digito de b. */
    int vai = 0;            /* Vai-um (carry): 0 ou 1. */
    int pos = 0;            /* Posicao atual no vetor de resultado invertido. */

    /* O resultado pode ter no maximo max(len_a, len_b) + 1 digitos
      (o +1 e para um possivel carry final). */
    int tam = (strlen(a) > strlen(b)) ? strlen(a) + 1 : strlen(b) + 1;

    /* Vetor auxiliar que armazenara os digitos em ordem INVERTIDA. */
    char *aux = malloc((tam + 1) * sizeof(char));

    /* Continua enquanto ainda houver digitos em algum dos numeros
     ou ainda existir um carry a ser processado. */
    while (i >= 0 || j >= 0 || vai) {
        int x = (i >= 0) ? (a[i] - '0') : 0; /* Digito atual de a (ou 0 se esgotou). */
        int y = (j >= 0) ? (b[j] - '0') : 0; /* Digito atual de b (ou 0 se esgotou). */

        int s = x + y + vai;       /* Soma dos digitos com o carry. */
        aux[pos++] = (s % 10) + '0'; /* Guarda o digito menos significativo da soma. */
        vai = s / 10;              /* O carry e 1 se a soma passou de 9, 0 caso contrario. */

        i--;
        j--;
    }

    aux[pos] = '\0';

    /* Aloca o resultado final e copia os digitos em ordem correta (invertendo aux). */
    char *r = malloc((pos + 1) * sizeof(char));
    for (int k = 0; k < pos; k++)
        r[k] = aux[pos - 1 - k];

    r[pos] = '\0';
    free(aux);
    return r;
}


/*
 * subtracaoInteiro: subtrai dois numeros inteiros positivos (a - b),
  podendo retornar um resultado negativo caso b > a.
  Algoritmo: semelhante a subtrai??o no papel.
    - Se b > a, troca os operandos e marca o resultado como negativo.
    - Percorre da direita para a esquerda, subtraindo digito a digito.
    - Usa "emprestimo" quando o digito de 'a' e menor que o de 'b'.
 */
char *subtracaoInteiro(char *a, char *b) {
    int neg = 0; /* Flag: 1 se o resultado for negativo. */

    /* Se a < b, a subtracao resultaria em negativo.
      Estrategia: calcula b - a e coloca o sinal '-' no resultado. */
    if (strlen(a) < strlen(b) ||
       (strlen(a) == strlen(b) && strcmp(a, b) < 0)) {
        char *t = a; a = b; b = t; /* Troca a e b. */
        neg = 1;
    }

    int i   = strlen(a) - 1; /* Ultimo digito de a (o maior). */
    int j   = strlen(b) - 1; /* Ultimo digito de b. */
    int emp = 0;              /* Emprestimo (borrow): 0 ou 1. */
    int pos = 0;

    /* O resultado tem no maximo o mesmo numero de digitos de a. */
    char *aux = malloc(strlen(a) + 1);

    while (i >= 0) {
        /* Digito atual de a, ja descontando o emprestimo da etapa anterior. */
        int x = (a[i] - '0') - emp;
        /* Digito atual de b (ou 0 se b ja foi totalmente percorrido). */
        int y = (j >= 0) ? (b[j] - '0') : 0;

        /* Se o digito de a (apos emprestimo) for menor que o de b,
         "pede emprestado" da casa a esquerda: adiciona 10 ao digito
          e sinaliza que a proxima etapa tera um emprestimo. */
        if (x < y) {
            x  += 10;
            emp = 1;
        } else {
            emp = 0;
        }

        aux[pos++] = (x - y) + '0';
        i--;
        j--;
    }

    /* Remove zeros a esquerda que possam ter ficado no resultado invertido
      (ex.: 100 - 99 = 001 invertido deve virar 1). */
    while (pos > 1 && aux[pos - 1] == '0')
        pos--;

    /* Aloca o resultado final; se for negativo, +1 para o sinal '-'. */
    char *r = malloc((pos + neg + 1) * sizeof(char));
    int k = 0;

    if (neg)
        r[k++] = '-';

    /* Inverte aux para obter os digitos na ordem correta. */
    for (int i = 0; i < pos; i++)
        r[k++] = aux[pos - 1 - i];

    r[k] = '\0';
    free(aux);
    return r;
}


/* OPERACOES PRINCIPAIS (COM SUPORTE A SINAL E DECIMAIS)*/

/*
  soma: soma dois numeros quaisquer (positivos, negativos, com ou sem
  casas decimais). Trata o sinal antes de delegar para somaInteiros().
 Regras de sinal aplicadas:
    (-a) + b   =  b - a
     a  + (-b) =  a - b
    (-a) + (-b) = -(a + b)
 
  Para numeros com decimais: antes de somar, alinha as casas decimais
  completando com zeros a direita o numero que tiver menos casas.
 */
char *soma(char *a, char *b) {

    /* (-a) + b = b - a */
    if (a[0] == '-' && b[0] != '-')
        return subtracaoInteiro(b, a + 1);

    /* a + (-b) = a - b */
    if (a[0] != '-' && b[0] == '-')
        return subtracaoInteiro(a, b + 1);

    /* (-a) + (-b) = -(a + b): soma os modulos e adiciona o sinal '-'. */
    if (a[0] == '-' && b[0] == '-') {
        char *tmp = soma(a + 1, b + 1);
        char *r   = malloc(strlen(tmp) + 2);
        r[0] = '-';
        strcpy(r + 1, tmp);
        free(tmp);
        return r;
    }

    /* --- Ambos positivos: alinhamento decimal e soma --- */

    /* Determina o maior numero de casas decimais entre os dois operandos. */
    int c1    = casasDecimais(a);
    int c2    = casasDecimais(b);
    int casas = (c1 > c2) ? c1 : c2;

    /* Cria copias para nao modificar as strings originais. */
    char *aa = malloc(strlen(a) + casas + 2);
    char *bb = malloc(strlen(b) + casas + 2);
    strcpy(aa, a);
    strcpy(bb, b);

    /* Remove os pontos: trabalhamos com inteiros internamente. */
    removePonto(aa);
    removePonto(bb);

    /* Completa com zeros a direita o numero com menos casas decimais,
      para que ambos fiquem com o mesmo "peso" decimal.
      Corrigindo: "150" e "225" (ambos representam duas casas decimais). */
    while (c1 < casas) { strcat(aa, "0"); c1++; }
    while (c2 < casas) { strcat(bb, "0"); c2++; }

    char *r = somaInteiros(aa, bb);

    free(aa);
    free(bb);

    /* Reinsere o ponto decimal no resultado. */
    r = colocaPonto(r, casas);

    return r;
}


/* subtracao: subtrai dois numeros quaisquer (a - b), com suporte a
 negativos e casas decimais.
  Regras de sinal aplicadas:
   (-a) - (-b) =  b - a
    (-a) -  b   = -(a + b)
   a  - (-b)  =  a + b
  Para numeros com decimais, o alinhamento e feito da mesma forma que
  na soma: completa com zeros e trabalha com inteiros internamente.
  Detalhe: se o resultado for negativo, o ponto decimal precisa ser
  reinserido apos o sinal '-', nao antes (ex.: "-0.05", nao "-.005").
 */
char *subtracao(char *a, char *b) {

    /* (-a) - (-b) = b - a */
    if (a[0] == '-' && b[0] == '-')
        return subtracaoInteiro(b + 1, a + 1);

    /* (-a) - b = -(a + b) */
    if (a[0] == '-' && b[0] != '-') {
        char *tmp = soma(a + 1, b);
        char *r   = malloc(strlen(tmp) + 2);
        r[0] = '-';
        strcpy(r + 1, tmp);
        free(tmp);
        return r;
    }

    /* a - (-b) = a + b */
    if (a[0] != '-' && b[0] == '-')
        return soma(a, b + 1);

    /* --- Ambos positivos: alinhamento decimal e subtracao --- */

    int c1    = casasDecimais(a);
    int c2    = casasDecimais(b);
    int casas = (c1 > c2) ? c1 : c2;

    char *aa = malloc(strlen(a) + casas + 2);
    char *bb = malloc(strlen(b) + casas + 2);
    strcpy(aa, a);
    strcpy(bb, b);

    removePonto(aa);
    removePonto(bb);

    while (c1 < casas) { strcat(aa, "0"); c1++; }
    while (c2 < casas) { strcat(bb, "0"); c2++; }

    char *r = subtracao(aa, bb);

    free(aa);
    free(bb);

    /* Se o resultado for negativo, e preciso reinserir o ponto decimal
      apos o sinal '-'. Exemplo: resultado interno "-5" com 2 casas
     decimais deve se tornar "-0.05", nao "-.05". */
    if (r[0] == '-') {
        char *tmp = malloc(strlen(r)); /* strlen(r) sem o '-' + '\0' */
        strcpy(tmp, r + 1);           /* Copia a parte numerica sem o sinal. */
        free(r);

        tmp = colocaPonto(tmp, casas); /* Insere o ponto na parte numerica. */

        char *f = malloc(strlen(tmp) + 2);
        f[0] = '-';
        strcpy(f + 1, tmp);
        free(tmp);

        return f;
    }

    r = colocaPonto(r, casas);
    return r;
}


/*
  multiplicacao: multiplica dois numeros quaisquer (positivos, negativos,
  com ou sem casas decimais).
  Algoritmo (multiplicacao longa, como no papel):
    1. Remove os sinais e determina se o resultado sera negativo
       (negativo se exatamente um dos fatores for negativo).
   2. Conta as casas decimais totais (soma das casas dos dois fatores).
    3. Remove os pontos e trabalha com inteiros.
   4. Para cada digito de bb (da direita para a esquerda):
      - Calcula o produto parcial: soma aa, d vezes (onde d e o digito).
      - Adiciona os zeros de deslocamento correspondentes a posicao do digito.
      - Acumula o produto parcial no resultado total.
    5. Reinsere o ponto decimal e o sinal no resultado.
 */
char *multiplicacao(char *a, char *b) {

    int neg = 0; /* Contador de sinais negativos (resultado negativo se impar). */

    /* Cria copias para nao alterar as strings originais. */
    char *aa = malloc(strlen(a) + 1);
    char *bb = malloc(strlen(b) + 1);
    strcpy(aa, a);
    strcpy(bb, b);

    /* Remove sinais e conta quantos havia.
      Regra: neg impar -> resultado negativo; neg par -> resultado positivo. */
    if (aa[0] == '-') { neg++; memmove(aa, aa + 1, strlen(aa)); }
    if (bb[0] == '-') { neg++; memmove(bb, bb + 1, strlen(bb)); }

    /* A quantidade de casas decimais do produto e a soma das casas dos fatores.
     Exemplo: 1.5 * 2.25 -> 1 + 2 = 3 casas decimais -> resultado tem 3 casas. */
    int casas = casasDecimais(aa) + casasDecimais(bb);

    /* Remove os pontos: trabalhamos apenas com inteiros na multiplicacao. */
    removePonto(aa);
    removePonto(bb);

    /* Resultado acumulado, inicializado com zero. */
    char *r = malloc(2);
    strcpy(r, "0");

    int tamB = strlen(bb);

    /* Itera sobre cada digito de bb, da direita (menos significativo) para a esquerda. */
    for (int i = tamB - 1; i >= 0; i--) {

        int d = bb[i] - '0'; /* Digito atual de bb convertido para inteiro. */

        /* Produto parcial de aa pelo digito d: soma aa d vezes.
          (Equivale a aa * d, mas usando apenas a funcao somaInteiros.) */
        char *parcial = malloc(2);
        strcpy(parcial, "0");

        for (int j = 0; j < d; j++) {
            char *tmp = somaInteiros(parcial, aa);
            free(parcial);
            parcial = tmp;
        }

        /* Aplica o deslocamento: o digito na posicao i conta da direita,
         entao seu produto parcial deve ser "deslocado" tamB-i-1 casas
          para a esquerda, o que equivale a adicionar zeros a direit. */
        int desloc = tamB - i - 1;
        int t = strlen(parcial);

        parcial = realloc(parcial, t + desloc + 1);

        for (int k = 0; k < desloc; k++)
            parcial[t + k] = '0';

        parcial[t + desloc] = '\0';

        /* Acumula o produto parcial no resultado total. */
        char *tmp = somaInteiros(r, parcial);
        free(r);
        free(parcial);
        r = tmp;
    }

    free(aa);
    free(bb);

    /* Reinsere o ponto decimal na posicao correta. */
    r = colocaPonto(r, casas);

    /* Adiciona o sinal negativo se necessario (e se o resultado nao for zero). */
    if (neg % 2 != 0 && strcmp(r, "0") != 0) {
        char *f = malloc(strlen(r) + 2);
        f[0] = '-';
        strcpy(f + 1, r);
        free(r);
        return f;
    }

    return r;
}


/*
 exponenciacao: calcula a^e, onde e e um expoente inteiro nao-negativo.
 
 * Limitacoes:
   - O expoente deve ser um inteiro (nao aceita 2.5, por exemplo).
   - Expoentes negativos nao sao suportados.
  Algoritmo: multiplicacao repetida (a * a * a ... e vezes).
  Comeca com resultado = 1 e multiplica pela base e vezes.
  Caso especial: qualquer numero elevado a 0 e 1.
 */
char *exponenciacao(char *a, char *b) {

    int e = atoi(b); /* Converte o expoente de string para inteiro. */

    if (e < 0) {
        char *r = malloc(30);
        strcpy(r, "Expoente invalido");
        return r;
    }

    /* Resultado inicial = 1 (qualquer numero elevado a 0 e 1). */
    char *r = malloc(2);
    strcpy(r, "1");

    if (e == 0)
        return r;

    /* Multiplica a base por ela mesma e vezes. */
    for (int i = 0; i < e; i++) {
        char *tmp = multiplicacao(r, a);
        free(r);
        r = tmp;
    }

    return r;
}


/*
  fatorial: calcula n! = 1 * 2 * 3 * ... * n.
 
 * Limitacoes:
    - Nao aceita numeros negativos (fatorial nao e definido).
    - Nao aceita numeros reais (fatorial so e definido para inteiros).
  Algoritmo: multiplicacao acumulada de 2 ate n.
  Comeca com resultado = 1 e multiplica por cada inteiro de 2 ate n.
  Caso especial: 0! = 1.
 */
char *fatorial(char *a) {

    if (a[0] == '-') {
        char *r = malloc(25);
        strcpy(r, "Nao existe");
        return r;
    }

    /* Verifica se o numero tem ponto decimal (seria um numero real). */
    if (strchr(a, '.') != NULL) {
        char *r = malloc(35);
        strcpy(r, "Fatorial so aceita inteiros");
        return r;
    }

    int n = atoi(a); /* Converte a string para inteiro. */

    /* Resultado inicial = 1 (0! = 1! = 1). */
    char *r = malloc(2);
    strcpy(r, "1");

    /* Multiplica o acumulado por cada inteiro de 2 ate n. */
    for (int i = 2; i <= n; i++) {
        char s[30];
        sprintf(s, "%d", i); /* Converte o inteiro i para string. */

        char *tmp = multiplicacao(r, s);
        free(r);
        r = tmp;
    }

    return r;
}


/* AVALIACAO DE EXPRESSOES */

/*
precedencia: retorna o nivel de prioridade de um operador.
  Prioridades definidas:
   '^' (exponenciacao) = 3  (maior prioridade)
    '*' (multiplicacao) = 2
    '+' e '-' (soma e subtracao) = 1  (menor prioridade)
    qualquer outro = 0 (invalido)
 */
int precedencia(char op) {
    if (op == '^') return 3;
    if (op == '*') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}


/*
  calcular: aplica o operador 'op' sobre os operandos 'a' e 'b' e
 retorna o resultado. Funciona como um despachante para as funcoes
 de cada operacao.
 */
char *calcular(char *a, char *b, char op) {
    if (op == '+') return soma(a, b);
    if (op == '-') return subtracao(a, b);
    if (op == '*') return multiplicacao(a, b);
    if (op == '^') return exponenciacao(a, b);
    return NULL;
}


/*
  expressao: avalia uma expressao matematica fornecida como string
  respeitando a precedencia dos operadores.
  Algoritmo: Shunting-Yard simplificado com duas pilhas.
    - Pilha de valores  (valores[]): armazena os numeros lidos.
    - Pilha de operadores (ops[]): armazena os operadores pendentes.
  Regra ao encontrar um operador novo:
    Antes de empilhar o operador atual, resolve todos os operadores
  na pilha que tenham precedencia maior ou igual ao operador atual.
    Isso garante que "*" seja calculado antes de "+", por exemplo.
 Ao final da expressao, resolve todos os operadores restantes na pilha.
 Limitacao: nao suporta parenteses nesta implementacao.
 */
char *expressao(char *o) {

    char *valores[1000]; /* Pilha de operandos (strings). */
    char  ops[1000];     /* Pilha de operadores (chars). */

    int topoV = -1; /* Topo da pilha de valores (-1 = vazia). */
    int topoO = -1; /* Topo da pilha de operadores (-1 = vazia). */

    int i = 0;

    while (o[i] != '\0') {

        /* --- Le um numero inteiro ou decimal completo --- */
        if (isdigit(o[i]) || o[i] == '.') {
            int inicio = i;

            /* Avanca ate o fim do numero (digitos e no maximo um ponto). */
            while (isdigit(o[i]) || o[i] == '.')
                i++;

            int   tam = i - inicio;
            char *num = malloc(tam + 1);
            strncpy(num, o + inicio, tam);
            num[tam] = '\0';

            valores[++topoV] = num; /* Empilha o numero lido. */
            continue;
        }

        /* --- Encontrou um operador --- */
        char op = o[i];

        /* Antes de empilhar o operador atual, desempilha e calcula todos
          os operadores no topo da pilha que tenham precedencia >= ao atual.
         Isso garante a ordem correta de avaliacao (ex.: * antes de +). */
        while (topoO >= 0 && precedencia(ops[topoO]) >= precedencia(op)) {
            char  operador = ops[topoO--];      /* Desempilha operador. */
            char *b_val    = valores[topoV--];  /* Segundo operando (mais recente). */
            char *a_val    = valores[topoV--];  /* Primeiro operando (mais antigo). */

            char *res = calcular(a_val, b_val, operador);

            free(a_val);
            free(b_val);

            valores[++topoV] = res; /* Empilha o resultado parcial. */
        }

        ops[++topoO] = op; /* Empilha o operador atual. */
        i++;
    }

    /* --- Resolve os operadores restantes na pilha --- */
    while (topoO >= 0) {
        char  operador = ops[topoO--];
        char *b_val    = valores[topoV--];
        char *a_val    = valores[topoV--];

        char *res = calcular(a_val, b_val, operador);

        free(a_val);
        free(b_val);

        valores[++topoV] = res;
    }

    /* O unico valor restante na pilha e o resultado final. */
    return valores[topoV];
}


/* =====================================================================
  FUNCAO PRINCIPAL
 * ===================================================================== */

int main() {
    int  op;
    char a[100000], b[100000], o[200000];
    char *res;

    do {
        printf("\n=== CALCULADORA DE NUMEROS GRANDES ===\n");
        printf("1 - Soma\n");
        printf("2 - Subtracao\n");
        printf("3 - Multiplicacao\n");
        printf("4 - Exponenciacao\n");
        printf("5 - Fatorial\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch (op) {
            case 1:
                printf("A: "); scanf("%s", a);
                printf("B: "); scanf("%s", b);
                res = soma(a, b);
                break;
            case 2:
                printf("A: "); scanf("%s", a);
                printf("B: "); scanf("%s", b);
                res = subtracao(a, b);
                break;
            case 3:
                printf("A: "); scanf("%s", a);
                printf("B: "); scanf("%s", b);
                res = multiplicacao(a, b);
                break;
            case 4:
                printf("Base: ");    scanf("%s", a);
                printf("Expoente: "); scanf("%s", b);
                res = exponenciacao(a, b);
                break;
            case 5:
                printf("Numero: "); scanf("%s", a);
                res = fatorial(a);
                break;
            case 0:
                printf("Encerrando...\n");
                continue;
            default:
                printf("Opcao invalida!\n");
                continue;
        }

        printf("Resultado: %s\n", res);
        free(res); /* Libera a memoria alocada dinamicamente pelo resultado. */

    } while (op != 0);

    /* --- Avaliacao de expressao livre --- */
    printf("\nDigite uma expressao (ex: 2^10+5*3): ");
    scanf("%s", o);

    res = expressao(o);
    printf("Resultado: %s\n", res);
    free(res);

    return 0;
}
