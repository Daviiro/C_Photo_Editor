#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <locale.h>

#define MAX_LENGH_COMAND 50
#define MAX_LENGH_COMMENT 100

/*Trabalho Final de PDI - 2020 - David Junior Rodrigues*/

typedef enum { false, true } bool; //criacao do tipo boleano//
FILE *inputFile; //vai ser o ponteiro designado a apontar para o arquivo da imagem de entrada//
FILE *txtFile; //vai ser o ponteiro designado a apontar para o arquivo de texto contendo os comandos//

/*cabecalhos das funcoes*/
void readHeader(FILE *f, short *largura, short *altura, short *tonsDeCinza, short *inicioDados); //funcao responsavel por ler o cabecalho da imagem de entrada, identificar a altura, largura, quantidade de tons de cinza e o comeco dos dados//
void printHeaderPGM(FILE *f, short largura, short altura, short tonsDeCinza); //funcao responsavel por imprimir o cabecalho PGM em algum arquivo//
void printHeaderPPM(FILE *f, short largura, short altura, short tonsDeCinza); //funcao responsavel por imprimir o cabecalho PPM em algum arquivo//
void controle(); //funcao responsavel por ser o menu/controle da aplicacao//
char * obterTipoArquivo(char endereco[]); //funcao responsavel por retornar o tipo do arquivo a partir do endereco do mesmo recebido//
void somaImagemValor(const char *tipo, unsigned char *matriz, short largura, short altura, float valor); //soma o valor passado em cada pixel da imagem//
void somaImagemComImagem(const char *tipo, unsigned char *matriz, short largura, short altura, FILE *f); //soma o valor pixel a pixel do arquivo passado com a imagem introduzida primeiramente//
void subtracaoImagemComImagem(const char *tipo, unsigned char *matriz, short largura, short altura, FILE *f); //faz a subtracao do endereco passado apos o comando de sub MENOS os valores encontrados na matriz da imagem intermediaria//
void fatiamentoSimples(const char *tipo, unsigned char *matriz, short largura, short altura, short A, short B, short ton); //realiza o fatiamento aonde altera o valor dentro do intervalo dado pelo usuario//
void fatiamento(const char *tipo, unsigned char *matriz, short largura, short altura, short A, short B, short tonDentro, short tonFora); //realiza o fatiamento na imagem com os 2 valores dos limites e dentro desse intervalo ele inserir certo valor de pixel, fora desse intervalo outro valor eh inserido//
void multiplicaImagemValor(const char *tipo, unsigned char *matriz, short largura, short altura, float valor); //multiplica cada pixel da imagem por um valor inserido pelo usuario//
void filtroMedia(const char *tipo, unsigned char *matriz, short largura, short altura, int tamFiltro); //aplica na matriz o filtro da media com o filtro tendo o tamanho especificado pelo usuario//
void shellSort(int *vet, int size); //ordena um vetor pelo metodo shellSort//
int retornaMediana(int *filtro, int tam); //recebe uma matriz "FILTRO" e retorna o valor da mediana contido no mesmo//
void filtroMediana(const char *tipo, unsigned char *matriz, short largura, short altura, int tamFiltro); //aplica o filtro da mediana de acordo com o tamanho passado pelo usuario//
void filtroLaplaciano(const char *tipo, unsigned char *matriz, short largura, short altura); //aplica o filtro laplaciano na matriz//
void equalizacaoHistograma(const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza); //aplica a equalizacao de histograma na matriz//
void extrairRGB(unsigned char *matriz, short largura, short altura, FILE *R, FILE *G, FILE *B); //cria 3 arquivos pgm a partir de um PPM, cada um desses pgm contendo um canal de cor, RGB//
void comporRGB(FILE *R, FILE *G, FILE *B); //pega 3 arquivos PGM e junta em um arquivo PPM com nome padrao//
void write(char *endereco, const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza, short inicioDados); //usa a matriz intermediaria para criar um arquivo de imagem em disco//
void controle(); // eh o menu da aplicacao//
void interativeMode(); //funcao responsavel por ler os comandos inseridos pelo usuario//
bool interpretador(unsigned char *comando, const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza, short incioDados);//funcao responsavel por interpretar o comando inserido pelo usuario ou lido do arquivo .txt, vai retornar TRUE caso o comando for realizado ou FALSE caso o comando nao exista//
void readingFileMode(); //funcao responsavel por ler os comandos encontrados no arquivo TXT//
short confereLimite(short valor); //funcao que recebe um valor e confere se ele eh maior que 0(caso for menor retorna 0)e menor que 255(caso for maior retorna 255)//
/*cabecalhos das funcoes*/

void printHeaderPPM(FILE *f, short largura, short altura, short tonsDeCinza)
{
    fprintf(f, "%s\n", "P3");
    fprintf(f, "%hi", largura);
    fprintf(f, " %hi\n", altura);
    fprintf(f, "%hi\n", tonsDeCinza);
    return;
}

void printHeaderPGM(FILE *f, short largura, short altura, short tonsDeCinza)
{
    fprintf(f, "%s\n", "P2");
    fprintf(f, "%hi", largura);
    fprintf(f, " %hi\n", altura);
    fprintf(f, "%hi\n", tonsDeCinza);
    return;
}

void readHeader(FILE *f, short *largura, short *altura, short *tonsDeCinza, short *inicioDados)
{
    fseek(f, 0, SEEK_SET); //faco com que o ponteiro do arquivo aponte para o comeco dele//
    unsigned char aux[5], comentario[MAX_LENGH_COMMENT];

    do
    {
        fscanf(f, "%s", aux); //leio os primeiros caracteres de uma linha do arquivo//
        if(!strcmp(aux, "#")) //caso eu achar um '#' que representa um comentario, eu pulo de linha e o ignoro//
        {
            fseek(f, -2, SEEK_CUR); //volto o ponteiro para pegar o '#' do comeco do comentario//
            fgets (comentario, MAX_LENGH_COMMENT, f); //leio a linha toda do comentario e o ponteiro atualiza para a proxima linha//
        }
    }while(!atoi(aux)); //o laco vai continuar operando ate que eu encontre um numeral inteiro, que representara a largura e posteriormente a altura e depois a quantidade de tons de cinza//

    *largura = atoi(aux); //altero os valores das variaveis de altura, largura e tons de cinza usando ponteiros, para que se altere o valor delas na funcao que chamou essa funcao aqui//
    fscanf(f, "%s", aux);
    *altura = atoi(aux);
    fscanf(f, "%s", aux);
    *tonsDeCinza = atoi(aux);

    *inicioDados = ftell(f); //pego a posicao referente logo ao inicio dos dados, apos o cabecalho//
    return;
}

short confereLimite(short valor)
{
    if(valor < 0)
    {
        return 0;
    }
    else if(valor >255)
    {
        return 255;
    }
    return valor;
}

void somaImagemValor(const char *tipo, unsigned char *matriz, short largura, short altura, float valor)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                *(matriz + col*largura + lin) = confereLimite(*(matriz + col*largura + lin) + valor);
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int lin, col, pro;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                for(pro=0; pro < 3; pro++) //laco FOR para andar pela PROFUNDIDADE, ou seja, os 3 canais de cores da imagem//
                {
                    *(matriz + col*largura*3 + lin*3 + pro) =  confereLimite(*(matriz + col*largura*3 + lin*3 + pro) + valor); // '3' eh o valor referente a profundidade da matriz que no caso sempre sera 3 por ter 3 canais de cores//
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void somaImagemComImagem(const char *tipo, unsigned char *matriz, short largura, short altura, FILE *f)
{
    int aux; //essa vai ser a variavel que vai receber o valor da segunda imagem que sera somado a matriz contendo a outra//
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                fscanf(f, "%i", &aux);
                *(matriz + col*largura + lin) = confereLimite(*(matriz + col*largura + lin) + aux);
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int lin, col, pro;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                for(pro=0; pro < 3; pro++) //laco FOR para andar pela PROFUNDIDADE, ou seja, os 3 canais de cores da imagem//
                {
                    fscanf(f, "%i", &aux);
                    *(matriz + col*largura*3 + lin*3 + pro) =  confereLimite(*(matriz + col*largura*3 + lin*3 + pro) + aux); // '3' eh o valor referente a profundidade da matriz que no caso sempre sera 3 por ter 3 canais de cores//
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void subtracaoImagemComImagem(const char *tipo, unsigned char *matriz, short largura, short altura, FILE *f)
{
    int aux; //essa vai ser a variavel que vai receber o valor da segunda imagem que sera somado a matriz contendo a outra//
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                fscanf(f, "%i", &aux);
                *(matriz + col*largura + lin) = confereLimite(aux - *(matriz + col*largura + lin));
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int lin, col, pro;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                for(pro=0; pro < 3; pro++) //laco FOR para andar pela PROFUNDIDADE, ou seja, os 3 canais de cores da imagem//
                {

                    fscanf(f, "%i", &aux);
                    *(matriz + col*largura*3 + lin*3 + pro) =  confereLimite(aux - *(matriz + col*largura*3 + lin*3 + pro)); // '3' eh o valor referente a profundidade da matriz que no caso sempre sera 3 por ter 3 canais de cores//
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void fatiamentoSimples(const char *tipo, unsigned char *matriz, short largura, short altura, short A, short B, short ton)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        short aux;
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                aux = *(matriz + col*largura + lin);
                if((aux>=A)&&(aux<=B))
                {
                    *(matriz + col*largura + lin) = ton;
                }
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int auxR, auxG, auxB;
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                auxR = *(matriz + col*largura*3 + lin*3 + 0);
                if((auxR>=A)&&(auxR<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 0) = ton;
                }

                auxG = *(matriz + col*largura*3 + lin*3 + 0);
                if((auxG>=A)&&(auxG<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 1) = ton;
                }

                auxB = *(matriz + col*largura*3 + lin*3 + 0);
                if((auxB>=A)&&(auxB<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 2) = ton;
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void fatiamento(const char *tipo, unsigned char *matriz, short largura, short altura, short A, short B, short tonDentro, short tonFora)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        int aux;
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                aux = *(matriz + col*largura + lin);
                if((aux>=A)&&(aux<=B))
                {
                    *(matriz + col*largura + lin) = tonDentro;
                }
                else
                {
                    *(matriz + col*largura + lin) = tonFora;
                }
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int auxR, auxG, auxB;
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                auxR = *(matriz + col*largura*3 + lin*3 + 0);
                if((auxR>=A)&&(auxR<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 0) = tonDentro;
                }
                else
                {
                    *(matriz + col*largura*3 + lin*3 + 0) = tonFora;
                }

                auxG = *(matriz + col*largura*3 + lin*3 + 1);
                if((auxG>=A)&&(auxG<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 1) = tonDentro;
                }
                else
                {
                    *(matriz + col*largura*3 + lin*3 + 1) = tonFora;
                }

                auxB = *(matriz + col*largura*3 + lin*3 + 2);
                if((auxB>=A)&&(auxB<=B))
                {
                    *(matriz + col*largura*3 + lin*3 + 2) = tonDentro;
                }
                else
                {
                    *(matriz + col*largura*3 + lin*3 + 2) = tonFora;
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void multiplicaImagemValor(const char *tipo, unsigned char *matriz, short largura, short altura, float valor)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                *(matriz + col*largura + lin) = confereLimite(*(matriz + col*largura + lin) * valor);
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int lin, col, pro;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha na primeira matriz//
            {
                for(pro=0; pro < 3; pro++) //laco FOR para andar pela PROFUNDIDADE, ou seja, os 3 canais de cores da imagem//
                {
                    *(matriz + col*largura*3 + lin*3 + pro) =  confereLimite(*(matriz + col*largura*3 + lin*3 + pro) * valor); // '3' eh o valor referente a profundidade da matriz que no caso sempre sera 3 por ter 3 canais de cores//
                }
            }
        }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void filtroMedia(const char *tipo, unsigned char *matriz, short largura, short altura, int tamFiltro)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        int filtro[tamFiltro][tamFiltro], media, auxlinha, auxcoluna;
        unsigned char matrizAux[altura][largura];
        int col, lin, i, j;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                media = 0; //tenho que zerar a media aqui para o valor usado no filtro anterior nao alterar o do proximo//
                auxcoluna = col - (tamFiltro/2);  //auxiliares de linha e coluna vao receber o valor da linha e coluna atuais MENOS o tamanho do filtro DIVIDIDO por 2(a parte inteira da divisao)//
                auxlinha = lin - (tamFiltro/2); // isso faz com que os auxiliares recebam a posicao da linha e coluna que correspodem a primeira posicao a ser inserida no filtro//
                for(i = 0; i < tamFiltro; i++) //laco FOR para andar(linha) pelo filtro e armazenar os valores no mesmo//
                {
                    for(j = 0; j < tamFiltro; j++) //laco FOR para andar(coluna) pelo filtro e armazenar os valores no mesmo//
                    {
                        if(auxcoluna >= 0 && auxcoluna < altura && auxlinha >= 0 && auxlinha < largura) //IF que ira perguntar se o valor nao saiu fora das bordas da imagem//
                        {
                            filtro[i][j] = *(matriz + auxcoluna*largura + auxlinha);
                        }
                        else
                        {
                            filtro[i][j] = 0;
                        }
                        auxlinha++;
                        media += filtro[i][j]; //calculando a media encontrada dentro do filtro//
                    }
                    auxcoluna++;
                    auxlinha = lin - (tamFiltro/2);
                }
                media = media / (tamFiltro*tamFiltro);
                matrizAux[col][lin] = media; //segunda matriz recebe o valor da media previamente calculada//
            }
        }
        /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                *(matriz + col*largura + lin) = matrizAux[col][lin];
            }
        }
        /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int filtro[tamFiltro][tamFiltro], media, auxlinha, auxcoluna;
        unsigned char matrizAux[altura][largura];
        int col, lin, i, j, profun=0;

          while(profun < 3) //fazer 3 vezes pois a matriz possui 3 dimensoes//
          {
            for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
            {
                for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
                {
                    media = 0; //tenho que zerar a media aqui para o valor usado no filtro anterior nao alterar o do proximo//
                    auxcoluna = col - (tamFiltro/2);  //auxiliares de linha e coluna vao receber o valor da linha e coluna atuais MENOS o tamanho do filtro DIVIDIDO por 2(a parte inteira da divisao)//
                    auxlinha = lin - (tamFiltro/2); // isso faz com que os auxiliares recebam a posicao da linha e coluna que correspodem a primeira posicao a ser inserida no filtro//
                    for(i = 0; i < tamFiltro; i++) //laco FOR para andar(linha) pelo filtro e armazenar os valores no mesmo//
                    {
                        for(j = 0; j < tamFiltro; j++) //laco FOR para andar(coluna) pelo filtro e armazenar os valores no mesmo//
                        {
                            if(auxcoluna >= 0 && auxcoluna < altura && auxlinha >= 0 && auxlinha < largura) //IF que ira perguntar se o valor nao saiu fora das bordas da imagem//
                            {
                                filtro[i][j] = *(matriz + auxcoluna*largura*3 + auxlinha*3 + profun);
                            }
                            else
                            {
                                filtro[i][j] = 0;
                            }
                            auxlinha++;
                            media += filtro[i][j]; //calculando a media encontrada dentro do filtro//
                        }
                        auxcoluna++;
                        auxlinha = lin - (tamFiltro/2);
                    }
                    media = media / (tamFiltro*tamFiltro);
                    matrizAux[col][lin] = media; //segunda matriz recebe o valor da media previamente calculada//
                }
            }
            /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
            for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
            {
                for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
                {
                    *(matriz + col*largura*3 + lin*3 + profun) = matrizAux[col][lin];
                }
            }
            /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
            profun++; ///incremento a variavel de profundidade///
          }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void shellSort(int *vet, int size) {
    int i , j , value;
    int gap = 1;
    while(gap < size) {
        gap = 3*gap+1;
    }
    while ( gap > 1) {
        gap /= 3;
        for(i = gap; i < size; i++) {
            value = vet[i];
            j = i - gap;
            while (j >= 0 && value < vet[j]) {
                vet [j + gap] = vet[j];
                j -= gap;
            }
            vet [j + gap] = value;
        }
    }
}

int retornaMediana(int *filtro, int tam){ //essa funcao recebe uma matriz, passa ela pra um vetor, ordena tal vetor e retorna o valor da mediana dele//
	int mediana, auxVetor[tam*tam], k=0;
	for(int i=0; i<tam; i++)
    {
        for(int j=0; j<tam; j++)
        {
            auxVetor[k++] = *(filtro + i*tam + j);
        }
    }
	shellSort(auxVetor, tam*tam);
	mediana = auxVetor[tam*tam/2];
	return mediana;
}

void filtroMediana(const char *tipo, unsigned char *matriz, short largura, short altura, int tamFiltro)
{
    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        int filtro[tamFiltro][tamFiltro], mediana, auxlinha, auxcoluna;
        unsigned char matrizAux[altura][largura];
        int col, lin, i, j;
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                auxcoluna = col - (tamFiltro/2);  //auxiliares de linha e coluna vao receber o valor da linha e coluna atuais MENOS o tamanho do filtro DIVIDIDO por 2(a parte inteira da divisao)//
                auxlinha = lin - (tamFiltro/2); // isso faz com que os auxiliares recebam a posicao da linha e coluna que correspodem a primeira posicao a ser inserida no filtro//
                for(i = 0; i < tamFiltro; i++) //laco FOR para andar(linha) pelo filtro e armazenar os valores no mesmo//
                {
                    for(j = 0; j < tamFiltro; j++) //laco FOR para andar(coluna) pelo filtro e armazenar os valores no mesmo//
                    {
                        if(auxcoluna >= 0 && auxcoluna < altura && auxlinha >= 0 && auxlinha < largura) //IF que ira perguntar se o valor nao saiu fora das bordas da imagem//
                        {
                            filtro[i][j] = *(matriz + auxcoluna*largura + auxlinha);
                        }
                        else
                        {
                            filtro[i][j] = 0;
                        }
                        auxlinha++;
                    }
                    auxcoluna++;
                    auxlinha = lin - (tamFiltro/2);
                }
                mediana = retornaMediana(&filtro[0][0], tamFiltro);
                matrizAux[col][lin] = mediana; //segunda matriz recebe o valor da mediana previamente calculada//
            }
        }
        /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                *(matriz + col*largura + lin) = matrizAux[col][lin];
            }
        }
        /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int filtro[tamFiltro][tamFiltro], mediana, auxlinha, auxcoluna;
        unsigned char matrizAux[altura][largura];
        int col, lin, i, j, profun=0;

          while(profun < 3) //fazer 3 vezes pois a matriz possui 3 dimensoes//
          {
            for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna da matriz//
            {
                for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
                {
                    auxcoluna = col - (tamFiltro/2);  //auxiliares de linha e coluna vao receber o valor da linha e coluna atuais MENOS o tamanho do filtro DIVIDIDO por 2(a parte inteira da divisao)//
                    auxlinha = lin - (tamFiltro/2); // isso faz com que os auxiliares recebam a posicao da linha e coluna que correspodem a primeira posicao a ser inserida no filtro//
                    for(i = 0; i < tamFiltro; i++) //laco FOR para andar(linha) pelo filtro e armazenar os valores no mesmo//
                    {
                        for(j = 0; j < tamFiltro; j++) //laco FOR para andar(coluna) pelo filtro e armazenar os valores no mesmo//
                        {
                            if(auxcoluna >= 0 && auxcoluna < altura && auxlinha >= 0 && auxlinha < largura) //IF que ira perguntar se o valor nao saiu fora das bordas da imagem//
                            {
                                filtro[i][j] = *(matriz + auxcoluna*largura*3 + auxlinha*3 + profun);
                            }
                            else
                            {
                                filtro[i][j] = 0;
                            }
                            auxlinha++;
                        }
                        auxcoluna++;
                        auxlinha = lin - (tamFiltro/2);
                    }
                    mediana = retornaMediana(&filtro[0][0], tamFiltro);
                    matrizAux[col][lin] = mediana; //segunda matriz recebe o valor da mediana previamente calculada//
                }
            }
            /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
            for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
            {
                for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
                {
                    *(matriz + col*largura*3 + lin*3 + profun) = matrizAux[col][lin];
                }
            }
            /*agora preciso passar os valores alterados contidos na matriz auxiliar para a matriz que vou usar no resto do programa*/
            profun++; ///incremento a variavel de profundidade///
          }
    }
    else ///caso eu nao indentifique o tipo nem como PGM nem como PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }
    return;
}

void filtroLaplaciano(const char *tipo, unsigned char *matriz, short largura, short altura)
{
    int lin, col, aux, i, j;
    int centro, cima, direita, baixo, esquerda;     // variáveis de auxílio, elas representam a Máscara que será aplicada na transformação //
    unsigned char matrizAux[altura][largura]; //matriz auxiliar

    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {

        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                // "centro", "cima", "direita", "baixo" e "esquerda" tomam suas respectivas posições para aplicarmos a máscara //
                centro = *(matriz + col*largura + lin);
                if((col > 0) && (col < (altura - 1)))
                {
                    cima = *(matriz + (col - 1)*largura + lin);
                    baixo = *(matriz + (col + 1)*largura + lin);
                }
                else
                {
                    cima = 0;
                    baixo = 0;
                }

                if((lin > 0) && (lin < (largura - 1)))
                {
                    direita = *(matriz + col*largura + (lin+1));
                    esquerda = *(matriz + col*largura + (lin-1));
                }
                else
                {
                    direita = 0;
                    esquerda = 0;
                }
                aux = centro;
                centro = 4*centro - (cima + direita + baixo + esquerda);
                aux = aux + centro;

                matrizAux[col][lin] = confereLimite(aux); // verificando limite antes de passar para a matriz//
            }
        }
        /*agora vou passar os dados da matriz auxiliar para a matriz que contem a imagem intermediaria que esta sendo usada ao longo do programa*/
        for(col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                *(matriz + col*largura + lin) = matrizAux[col][lin];
            }
        }
        /*agora vou passar os dados da matriz auxiliar para a matriz que contem a imagem intermediaria que esta sendo usada ao longo do programa*/
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") ) ///caso eu esteja manipulando uma imagem do tipo PPM///
    {
        int profun=0;
        while(profun<3)
        {
            for(col=0; col < altura; col++)
            {
                for(lin=0; lin < largura; lin++)
                {
                    // "centro", "cima", "direita", "baixo" e "esquerda" tomam suas respectivas posições para aplicarmos a máscara //
                    centro = *(matriz + col*largura*3 + lin*3 + profun);
                    if((col > 0) && (col < (altura - 1)))
                    {
                        cima = *(matriz + (col - 1)*largura*3 + lin*3 + profun);
                        baixo = *(matriz + (col + 1)*largura*3 + lin*3 + profun);
                    }
                    else
                    {
                        cima = 0;
                        baixo = 0;
                    }

                    if((lin > 0) && (lin < (largura - 1)))
                    {
                        direita = *(matriz + col*largura*3 + (lin+1)*3 + profun);
                        esquerda = *(matriz + col*largura*3 + (lin-1)*3 + profun);
                    }
                    else
                    {
                        direita = 0;
                        esquerda = 0;
                    }
                    aux = centro;
                    centro = 4*centro - (cima + direita + baixo + esquerda);
                    aux = aux + centro;

                    matrizAux[col][lin] = confereLimite(aux); // verificando limite antes de passar para a matriz//
                }
            }
            /*agora vou passar os dados da matriz auxiliar para a matriz que contem a imagem intermediaria que esta sendo usada ao longo do programa*/
            for(col=0; col < altura; col++)
            {
                for(lin=0; lin < largura; lin++)
                {
                    *(matriz + col*largura*3 + lin*3 + profun) = matrizAux[col][lin];
                }
            }
            /*agora vou passar os dados da matriz auxiliar para a matriz que contem a imagem intermediaria que esta sendo usada ao longo do programa*/
            profun++; ///atualizo a variavel de profundidade///
        }
    }
    else ///caso eu esteja manipulando um tipo de arquivo que nao se enquadre nem em PGM nem em PPM///
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }

    return;
}

void equalizacaoHistograma(const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza)
{
    int L = 256, novaTonalidade[tonsDeCinza], antigaTonalidade, i, tons[tonsDeCinza], aux;
    float Pr=0;

    for(i=0; i<256; i++)
    {
        tons[i]=0;
        novaTonalidade[i]=0;
    }

    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso eu esteja manipulando uma imagem do tipo PGM///
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                aux = *(matriz + col*largura + lin);
                tons[aux]++;
            }
        }
        for(i=0; i<256; i++)
        {
            if(tons[i] != 0)
            {
                Pr += tons[i]/(float)(altura*largura);
                novaTonalidade[i] = confereLimite((L-1)*Pr);
            }
        }
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                antigaTonalidade = *(matriz + col*largura + lin);
                *(matriz + col*largura + lin) = novaTonalidade[antigaTonalidade];
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm") )
    {
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                aux = *(matriz + col*largura*3 + lin*3 + 0);
                tons[aux]++;
                aux = *(matriz + col*largura*3 + lin*3 + 1);
                tons[aux]++;
                aux = *(matriz + col*largura*3 + lin*3 + 2);
                tons[aux]++;
            }
        }
        for(i=0; i<256; i++)
        {
            if(tons[i] != 0)
            {
                Pr += tons[i]/(float)(altura*largura);
                novaTonalidade[i] = confereLimite((L-1)*Pr);
            }
        }
        for(int col=0 ; col <  altura; col++) //laco FOR para andar pelos valores da coluna na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para andar pelos valores da linha da matriz//
            {
                antigaTonalidade = *(matriz + col*largura*3 + lin*3 + 0);
                *(matriz + col*largura*3 + lin*3 + 0) = novaTonalidade[antigaTonalidade];
                antigaTonalidade = *(matriz + col*largura*3 + lin*3 + 1);
                *(matriz + col*largura*3 + lin*3 + 1) = novaTonalidade[antigaTonalidade];
                antigaTonalidade = *(matriz + col*largura*3 + lin*3 + 2);
                *(matriz + col*largura*3 + lin*3 + 2) = novaTonalidade[antigaTonalidade];
            }
        }
    }
    else
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }

    return;
}

void extrairRGB(unsigned char *matriz, short largura, short altura, FILE *R, FILE *G, FILE *B)
{
    for(int col=0 ; col <  altura; col++) //laco FOR para armazenar os valores da linha na primeira matriz//
    {
        for(int lin=0 ; lin < largura; lin++) //laco FOR para armazenar os valores da coluna na primeira matriz//
        {
            fprintf(R, "%i\n", *(matriz + col*largura*3 + lin*3 + 0));
            fprintf(G, "%i\n", *(matriz + col*largura*3 + lin*3 + 1));
            fprintf(B, "%i\n", *(matriz + col*largura*3 + lin*3 + 2));
        }
    }
    return;
}

void comporRGB(FILE *R, FILE *G, FILE *B)
{
    FILE *arquivo;
    short largura, altura, tonsDeCinza, inicioDados;
    readHeader(R, &largura, &altura, &tonsDeCinza, &inicioDados);
    readHeader(G, &largura, &altura, &tonsDeCinza, &inicioDados);
    readHeader(B, &largura, &altura, &tonsDeCinza, &inicioDados);

    int aux;
    if((arquivo = fopen("compostoRGB.ppm", "w")) == NULL)
    {
        printf("\n\tErro na criacao do arquivo que conteria a composicao RGB");
        return;
    }
    printHeaderPPM(arquivo, largura, altura, tonsDeCinza);
    while(!feof(R) && !feof(G) && !feof(B))
    {
        fscanf(R, "%i", &aux);
        fprintf(arquivo, "%i\n", aux);

        fscanf(G, "%i", &aux);
        fprintf(arquivo, "%i\n", aux);

        fscanf(B, "%i", &aux);
        fprintf(arquivo, "%i\n", aux);
    }

    printf("\n\tO arquivo 'compostoRGB.ppm' foi criado e contem a composicao dos 3 arquivos RGB passados.\n");
    fclose(arquivo); //fecho o arquivo//
    return;
}

void write(char *endereco, const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza, short inicioDados)
{
    FILE *outputFile;
    outputFile = fopen(endereco, "w");

    if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) ///caso o arquivo for do tipo PGM///
    {
        printHeaderPGM(outputFile, largura, altura, tonsDeCinza);

        for(int col=0 ; col <  altura; col++) //laco FOR para armazenar os valores da linha na primeira matriz//
        {
            for(int lin=0 ; lin < largura; lin++) //laco FOR para armazenar os valores da coluna na primeira matriz//
            {
                fprintf(outputFile, "%d\n", *(matriz + col*largura + lin));
            }
        }
    }
    else if(!strcmp(tipo, "ppm") || !strcmp(tipo, ".ppm")) ///caso o arquivo for do tipo PPM///
    {
        printHeaderPPM(outputFile, largura, altura, tonsDeCinza);
        int lin, col, pro;

        for(col=0 ; col <  altura; col++) //laco FOR para armazenar os valores da linha na primeira matriz//
        {
            for(lin=0 ; lin < largura; lin++) //laco FOR para armazenar os valores da coluna na primeira matriz//
            {
                for(pro=0; pro < 3; pro++)
                {
                    fprintf(outputFile, "%d\n", *(matriz + col*largura*3 + lin*3 + pro)); // '3' eh o valor referente a profundidade da matriz que no caso sempre sera 3 por ter 3 canais de cores//
                }
            }
        }
    }
    else
    {
        printf("\n\tO tipo de arquivo inserido nao eh nem .pgm nem .ppm, logo eh incompativel com essa aplicacao");
    }

    printf("\t");
    system("PAUSE");
    fclose(outputFile);
    return;
}

bool interpretador(unsigned char *comando, const char *tipo, unsigned char *matriz, short largura, short altura, short tonsDeCinza, short inicioDados)
{
    //printf("\n\n\tComando recebido %s", comando); //teste

    /*comandos que possuem soh uma palavra*/
    if(!strcmp(comando, "end"))
    {
        return true;
    }
    else if(!strcmp(comando, "dados"))
    {
        printf("\n\t\tLargura: %hi", largura);
        printf("\n\t\tAltura: %hi", altura);
        printf("\n\t\tTons De Cinza: %hi", tonsDeCinza);
        printf("\n\t\tInicio dos dados: %hi\n", inicioDados);
        return true;
    }
    else if(!strcmp(comando, "filtro-laplaciano"))
    {
        filtroLaplaciano(tipo, matriz, largura, altura);
        return true;
    }
    else if(!strcmp(comando, "equalizacao"))
    {
        equalizacaoHistograma(tipo, matriz, largura, altura, tonsDeCinza);
        return true;
    }
    else if(!strcmp(comando, "h") ||!strcmp(comando, "H") )
    {
         printf("\t+-----------------------------------------------------------------------+\n");
         printf("\t|  .::Comandos::.       |             .::Funcao::.                      |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  dados                |         exibe dados da imagem                 |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  end                  |         encerra                               |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  soma \"inteiro/real\"  |         soma um valor na imagem               |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  sub  \"inteiro/real\"  |         subtrai um valor na imagem            |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  multi  \"inteiro/real\"|         multiplica um valor na imagem         |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  soma \"endereco.algo\" |         soma 2 imagens                        |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  sub \"endereco.algo\"  |         subtrai 2 imagens                     |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  fatiamento-simples   |         usa os dois primeiros inteiros como   |\n");
         printf("\t|  \"inteiro\" \"inteiro\"  |         intervalo para fazer o fatiamento e   |\n");
         printf("\t|  \"inteiro\"            |         o terceiro valor vai ser o tom de     |\n");
         printf("\t|                       |         cinza dentro do intervalo             |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  fatiamento           |         usa os dois primeiros inteiros como   |\n");
         printf("\t|  \"inteiro\" \"inteiro\"  |         intervalo para fazer o fatiamento,    |\n");
         printf("\t|  \"inteiro\" \"inteiro\"  |         o terceiro valor vai ser o tom de     |\n");
         printf("\t|                       |         cinza dentro do intervalo e o quarto  |\n");
         printf("\t|                       |         vai ser o valor de fora               |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  filtro-media         |         aplica o filtro da média com o tamanho|\n");
         printf("\t|  \"inteiro\"            |         do inteiro passado                    |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  filtro-mediana       |         aplica o filtro da mediana com o      |\n");
         printf("\t|  \"inteiro\"            |         tamanho do inteiro passado            |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  filtro-laplaciano    |         aplica o filtro laplaciano            |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  extrair              |         Extrai o ppm em RGB nos 3 arquivos pgm|\n");
         printf("\t|  \"endereco.pgm\"       |         informados                            |\n");
         printf("\t|  \"endereco.pgm\"       |                                               |\n");
         printf("\t|  \"endereco.pgm\"       |                                               |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  compor               |         Compõe os 3 arquivos pgm em um arquivo|\n");
         printf("\t|  \"endereco.pgm\"       |         ppm padrão                            |\n");
         printf("\t|  \"endereco.pgm\"       |                                               |\n");
         printf("\t|  \"endereco.pgm\"       |                                               |\n");
         printf("\t+-----------------------+-----------------------------------------------+\n");
         printf("\t|  escreva              |         Escreve a imagem no disco de acordo   |\n");
         printf("\t|  \"endereco.pgm\"       |         com o endereco informado              |\n");
         printf("\t+-----------------------------------------------------------------------+\n");

         printf("\n\tOBS: Sempre separar o comando de um valor/endereço usando um Espaço.\n\tAs aspas foram usadas na tabela para melhor visualização somente.\n\tO mesmo ocorre com as quebras de linhas na tabela.\n");
        return true;
    }
    /*comandos que possuem soh uma palavra*/

    /*comandos que possuem MAIS de uma palavra*/
    char* token;
    token = strtok(comando, " ");
    if(!strcmp(token, "soma"))
    {
        token = strtok(NULL, " ");
        if(atoi(token))
        {
            float soma = atof(token);
            somaImagemValor(tipo, matriz, largura, altura, soma);
            //printf("\n\n\tLogo eh pra somar %f\n", soma);
            return true;
        }
        else{
            FILE *arquivo;
            short larguraAux, alturaAux, tonsDeCinzaAux, inicioDadosAux; //sao as variaveis auxiliares que iram corresponder a dados da segunda imagem escolhida//
            //printf("\n\n\tNesse caso vai somar com outra imagem.");
            //printf("\n\nO endereco dessa imagem seria ...%s...", token);
            if((arquivo = fopen(token, "r")) == NULL)
            {
                printf("\n\tNao foi possivel abrir a segunda imagem.");
                return false;
            }
            readHeader(arquivo, &larguraAux, &alturaAux, &tonsDeCinzaAux, &inicioDadosAux);
            if(largura != larguraAux || altura != alturaAux)
            {
                printf("\n\tNao foi fazer a soma pois as imagens possuem diferentes medidas de altura ou largura.");
                return false;
            }
            somaImagemComImagem(tipo, matriz, largura, altura, arquivo);
            fclose(arquivo); //fecho o arquivo da imagem que foi somada//
            return true;
        }
    }
    else if(!strcmp(token, "sub"))
    {
        token = strtok(NULL, " ");
        if(atoi(token))
        {
            float sub = (-1)*atof(token);
            somaImagemValor(tipo, matriz, largura, altura, sub);
            //printf("\n\n\tLogo eh pra subtrair %i\n", sub);
            return true;
        }
        else{
            FILE *arquivo;
            short larguraAux, alturaAux, tonsDeCinzaAux, inicioDadosAux; //sao as variaveis auxiliares que iram corresponder a dados da segunda imagem escolhida//
            //printf("\n\n\tNesse caso vai subtrair com outra imagem.");
            //printf("\n\nO endereco dessa imagem seria ...%s...", token);
            if((arquivo = fopen(token, "r")) == NULL)
            {
                printf("\n\tNao foi possivel abrir a segunda imagem.");
                return false;
            }
            readHeader(arquivo, &larguraAux, &alturaAux, &tonsDeCinzaAux, &inicioDadosAux);
            if(largura != larguraAux || altura != alturaAux)
            {
                printf("\n\tNao foi fazer a subtracao pois as imagens possuem diferentes medidas de altura ou largura.");
                return false;
            }
            subtracaoImagemComImagem(tipo, matriz, largura, altura, arquivo);
            fclose(arquivo); //fecho o arquivo da imagem que foi somada//
            return true;
        }
    }
    else if(!strcmp(token, "multi"))
    {
        token = strtok(NULL, " ");
        float multi = atof(token);
        multiplicaImagemValor(tipo, matriz, largura, altura, multi);
        //printf("\n\n\tLogo eh pra multiplicar %f\n", multi);
        return true;
    }
    else if(!strcmp(token, "fatiamento-simples"))
    {
        short aux;
        token = strtok(NULL, " ");
        short A = confereLimite(atoi(token));
        token = strtok(NULL, " ");
        short B = confereLimite(atoi(token));
        token = strtok(NULL, " ");
        short ton = confereLimite(atoi(token));

        /*conferindo se A <= B*/
        if(A > B)
        {
            aux = A;
            A = B;
            B = aux;
        }
        /*conferindo se A <= B*/
        fatiamentoSimples(tipo, matriz, largura, altura, A, B, ton);
        return true;
    }
    else if(!strcmp(token, "fatiamento"))
    {
        short aux;
        token = strtok(NULL, " ");
        int A = atoi(token);
        token = strtok(NULL, " ");
        int B = atoi(token);
        token = strtok(NULL, " ");
        short ton = confereLimite(atoi(token));
        token = strtok(NULL, " ");
        short ton2 = confereLimite(atoi(token));

        /*conferindo se A <= B*/
        if(A > B)
        {
            aux = A;
            A = B;
            B = aux;
        }
        /*conferindo se A <= B*/

        fatiamento(tipo, matriz, largura, altura, A, B, ton, ton2);
        return true;
    }
    else if(!strcmp(token, "filtro-media"))
    {
        token = strtok(NULL, " ");
        int tam = atoi(token);
        filtroMedia(tipo, matriz, largura, altura, tam);
        return true;
    }
    else if(!strcmp(token, "filtro-mediana"))
    {
        token = strtok(NULL, " ");
        int tam = atoi(token);
        filtroMediana(tipo, matriz, largura, altura, tam);
        return true;
    }
    else if(!strcmp(token, "extrair"))
    {
        //primeiramente confiro se o arquivo eh um ppm//
        if(strcmp(tipo, "ppm") && strcmp(tipo, ".ppm"))
        {
            printf("\n\n\tNao eh possivel extrair RGB de uma imagem que nao seja .ppm.");
            return false;
        }

        FILE *R, *G, *B;
        token = strtok(NULL, " ");
        if((R = fopen(token, "w")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao R.");
            return false;
        } //vai conter o endereco que vai conter a parte R do RGB//
        token = strtok(NULL, " ");
        if((G = fopen(token, "w")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao G.");
            return false;
        } //vai conter o endereco que vai conter a parte G do RGB//
        token = strtok(NULL, " ");
        if((B = fopen(token, "w")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao B.");
            return false;
        } //vai conter o endereco que vai conter a parte B do RGB//

        /*imprimo o cabecalho referente a PGM nos 3 arquivos*/
        printHeaderPGM(R, largura, altura, tonsDeCinza);
        printHeaderPGM(G, largura, altura, tonsDeCinza);
        printHeaderPGM(B, largura, altura, tonsDeCinza);
        /*imprimo o cabecalho referente a PGM nos 3 arquivos*/

        extrairRGB(matriz, largura, altura, R, G, B);

        /*Fecho os arquivos*/
        fclose(R);
        fclose(G);
        fclose(B);
        /*Fecho os arquivos*/
        return true;
    }
    else if(!strcmp(token, "compor"))
    {
        FILE *R, *G, *B;
        token = strtok(NULL, " ");
        if((R = fopen(token, "r")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao R.");
            return false;
        } //vai conter o endereco que vai conter a parte R do RGB//
        token = strtok(NULL, " ");
        if((G = fopen(token, "r")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao G.");
            return false;
        } //vai conter o endereco que vai conter a parte G do RGB//
        token = strtok(NULL, " ");
        if((B = fopen(token, "r")) == NULL)
        {
            printf("\n\tNao foi possivel abrir a imagem correspondente ao B.");
            return false;
        } //vai conter o endereco que vai conter a parte B do RGB//

        comporRGB(R, G, B);
        return true;
    }
    else if(!strcmp(token, "escreva"))
    {
        token = strtok(NULL, " ");
        char endereco[MAX_LENGH_COMAND];
        strcpy(endereco, token);
        printf("\n\tArquivo foi salvo em: %s.\n", endereco);
        write(endereco, tipo, matriz, largura, altura, tonsDeCinza, inicioDados);
        return true;
    }
    /*comandos que possuem MAIS de uma palavra*/

    return false;
}

char * obterTipoArquivo(char endereco[])
{
    char *tipo;
    int len = strlen(endereco);

    strncpy(tipo, endereco + (len-3), 3);

    //char* tipo;
    //tipo = strtok(endereco, ".");
    //tipo = strtok(NULL, "");
    /*
    char * tipo = (char*) malloc(4 * sizeof(char))  ;
    strncpy (tipo, &endereco[strlen(endereco)-3], 3 );
    */
    return tipo;
}

void interativeMode()
{
    char endereco[MAX_LENGH_COMAND];

    do{
    printf("\n\n\tDigite o nome do arquivo PGM ou PPM que deseja abrir.\n");
    printf("\tOBS: O arquivo deve estar na mesma pasta do programa ou deve-se especificar seu endereco completo.\n");
    printf("\tNao se esqueca tambem da extencao .pgm ou .ppm do arquivo.\n");
    printf("\tExemplo: C:/imagens/lena256.pgm\n");
    printf("\tEndereco: ");

    /*pego o endereco da imagem de entrada dado pelo usuario e abro ela*/
        gets(endereco); fflush(stdin);
        inputFile = fopen(endereco, "r");
        if(inputFile == NULL)
        {
            printf("\n\tErro na abertura do arquivo!");
            printf("\n\tO endereco inserido esta incorreto.");
            printf("\n\tTente novamente.");
            printf("\n\n\t");
            system("PAUSE");
            system("cls");
        }
        else
        {
            printf("\n\tAbertura do arquivo realizada com sucesso!");
            printf("\n\t");
        }
    }while(inputFile == NULL);
    /*pego o endereco da imagem de entrada dado pelo usuario e abro ela*/

    /*devo agora identificar o tipo de arquivo com qual vou mexer, PGM ou PPM*/
    char * tipo = obterTipoArquivo(endereco);
    printf("\n\tO tipo do arquivo foi identificado como sendo .%s\n", tipo);
    /*devo agora identificar o tipo de arquivo com qual vou mexer, PGM ou PPM*/

    short altura, largura, tonsDeCinza, inicioDados; //variaveis que armazenaram os dados do arquivo de entrada "inputFile"//
    readHeader(inputFile, &largura, &altura, &tonsDeCinza, &inicioDados); //leio o cabecalho do arquivo de entrada "inputFile" para armazenar a altura, largura, tons de cinza e inicio dos dados dele//

    unsigned char comando[MAX_LENGH_COMAND];
    short i=0;

    printf("\n\tAgora insira os comandos que devem ser realizados.");
    printf("\n\tPara ajuda, digite H, isso vai exibir a lista de comandos disponiveis.");
    printf("\n\tQuando acabar todos os processamentos desejados digite \"end\".\n");
    if(!strcmp(tipo, "pgm")) //caso estiver trabalhando com arquivo pgm
    {
        unsigned char matrizIntermediariaPGM[altura][largura]; //matriz que vai conter valores da imagem PGM enquanto ela esta passando pelos processamentos//

        for(int col=0 ; col <  altura && !feof(inputFile); col++)
        {
            for(int lin=0 ; lin < largura && !feof(inputFile); lin++)
            {
                fscanf(inputFile,"%d", &matrizIntermediariaPGM[col][lin]);
            }
        }

        do{
            printf("\t[%i] - ", i++);
            gets(comando);
            fflush(stdin);

            if(interpretador(comando, tipo, &matrizIntermediariaPGM[0][0], largura, altura, tonsDeCinza, inicioDados))
            {
                printf("\t\tComando Realizado.\n");
            }
            else{
                printf("\n\t\tComando Invalido.\n");
            }
        }while(strcmp(comando,"end"));

    }
    else if(!strcmp(tipo, "ppm")) //caso estiver trabalhando com arquivo ppm
    {
        unsigned char matrizIntermediariaPPM[altura][largura][3]; //matriz que vai conter valores da imagem PPM enquanto ela esta passando pelos processamentos//

        for(int col=0 ; col <  altura && !feof(inputFile); col++)
        {
            for(int lin=0 ; lin < largura && !feof(inputFile); lin++)
            {
                for(int pro=0; pro < 3; pro++)
                {
                    fscanf(inputFile,"%d", &matrizIntermediariaPPM[col][lin][pro]);
                }
            }
        }

        do{
            printf("\t[%i] - ", i++);
            gets(comando);
            fflush(stdin);

            if(interpretador(comando, tipo, &matrizIntermediariaPPM[0][0][0], largura, altura, tonsDeCinza, inicioDados))
            {
                printf("\t\tComando Realizado.\n");
            }
            else{
                printf("\n\t\tComando Invalido.\n");
            }
        }while(strcmp(comando,"end"));
    }
    else
    {
        printf("\n\tO tipo de arquivo inserido e incompativel com essa aplicacao.");
    }


    return;
}

void readingFileMode()
{
    txtFile = fopen("comandos.txt", "r");
    char comando[MAX_LENGH_COMAND];
    char* token;

    if(txtFile == NULL)
    {
        printf("\n\tErro na abertura do arquivo comandos.txt!");
        printf("\n\tO arquivo não foi encontrado.");
        printf("\n\tTente novamente.");
        printf("\n\n\t");
        system("PAUSE");
        system("cls");
    }
    else
    {
        printf("\n\tAbertura do arquivo comandos.txt realizada com sucesso!");
        printf("\n\t");
    }

    fgets(comando, MAX_LENGH_COMAND, txtFile);  printf("%s", comando);

    token = strtok(comando, " "); //se os comandos estiverem corretos no arquivo .txt esse primeiro token deve ter a palavra "leitura" armazenada nele//
    if(!strcmp(token, "leitura"))
    {
        token = strtok(NULL, "\n"); //esse token deve conter o endereco do arquivo de entrada, ou em outras palavras o arquivo q sera apontado por "inputFile"//

        /*abrindo o arquivo inputFile*/
        inputFile = fopen(token, "r");
        if(inputFile == NULL)
        {
            printf("\n\tErro na abertura do arquivo (%s)!", token);
            printf("\n\tO endereco inserido esta incorreto.");
            printf("\n\tTente novamente.");
            printf("\n\n\t");
            system("PAUSE");
            system("cls");
        }
        else
        {
            printf("\n\tAbertura do arquivo realizada com sucesso!");
            printf("\n\t");
        }
        /*abrindo o arquivo inputFile*/

        /*devo agora identificar o tipo de arquivo com qual vou mexer, PGM ou PPM*/
        char * tipo = obterTipoArquivo(token);
        printf("\n\tO tipo do arquivo foi identificado como sendo %s\n", tipo); //teste
        /*devo agora identificar o tipo de arquivo com qual vou mexer, PGM ou PPM*/

        short altura, largura, tonsDeCinza, inicioDados; //variaveis que armazenaram os dados do arquivo de entrada "inputFile"//
        readHeader(inputFile, &largura, &altura, &tonsDeCinza, &inicioDados); //leio o cabecalho do arquivo de entrada "inputFile" para armazenar a altura, largura, tons de cinza e inicio dos dados dele//

        if(!strcmp(tipo, "pgm") || !strcmp(tipo, ".pgm")) //caso estiver trabalhando com arquivo pgm
        {
            unsigned char matrizIntermediariaPGM[altura][largura]; //matriz que vai conter valores da imagem PGM enquanto ela esta passando pelos processamentos//

            for(int col=0 ; col <  altura && !feof(inputFile); col++)
            {
                for(int lin=0 ; lin < largura && !feof(inputFile); lin++)
                {
                    fscanf(inputFile,"%d", &matrizIntermediariaPGM[col][lin]);
                }
            }

            char comandos2[MAX_LENGH_COMAND];
            do{

                //printf("\t1-O tipo do arquivo foi identificado como sendo @@%s@@", tipo); //teste
                fgets(comandos2, MAX_LENGH_COMAND, txtFile);
                //fscanf(txtFile, "%[^\n]", &comando);
                //fseek(txtFile, 2, SEEK_CUR);
                int len = strlen(comandos2);
                if ((len > 0) && (comandos2[len - 1] == '\n')) {
                    comandos2[--len] = 0; // remove quebra de linha e actualiza len
                }

                printf("\n\t Comando %s.\n", comandos2);
                //printf("\t2-O tipo do arquivo foi identificado como sendo @@%s@@", tipo); //teste
                system("PAUSE");

                if(interpretador(comandos2, tipo, &matrizIntermediariaPGM[0][0], largura, altura, tonsDeCinza, inicioDados))
                {
                    printf("\n\t\tComando Realizado.\n");
                }
                else{
                    printf("\n\t\tComando Invalido.\n");
                }
            }while(strcmp(comandos2,"end"));

        }
        else if(!strcmp(tipo, "ppm") || !strcmp(tipo, "ppm")) //caso estiver trabalhando com arquivo ppm
        {
            unsigned char matrizIntermediariaPPM[altura][largura][3]; //matriz que vai conter valores da imagem PPM enquanto ela esta passando pelos processamentos//

            for(int col=0 ; col <  altura && !feof(inputFile); col++)
            {
                for(int lin=0 ; lin < largura && !feof(inputFile); lin++)
                {
                    for(int pro=0; pro < 3; pro++)
                    {
                        fscanf(inputFile,"%d", &matrizIntermediariaPPM[col][lin][pro]);
                    }
                }
            }

            char comandos2[MAX_LENGH_COMAND];
            do{
                fgets(comandos2, MAX_LENGH_COMAND, txtFile);
                int len = strlen(comandos2);
                if ((len > 0) && (comandos2[len - 1] == '\n')) {
                    comandos2[--len] = 0; // remove quebra de linha e actualiza len
                }
                printf("\n\t Comando %s", comandos2);

                if(interpretador(comandos2, tipo, &matrizIntermediariaPPM[0][0][0], largura, altura, tonsDeCinza, inicioDados))
                {
                    printf("\n\t\tComando Realizado.\n");
                }
                else{
                    printf("\n\t\tComando Invalido.\n");
                }
            }while(strcmp(comandos2,"end"));
        }
        else
        {
            printf("\n\tO tipo de arquivo inserido e incompativel com essa aplicacao.");
        }

    }
    else
    {
        printf("\n\tOcorreu um erro pelo primeiro comando contido no arquivo .txt nao corresponder a um comando de leitura.");
        return;
    }

    fclose(txtFile);
    return;
}

void controle()
{
    bool continuar;
    unsigned char escolha;

    do{
    printf("\n\tEscolha como o programa deve interpretar os comandos:");
    printf("\n\tI - Modo Interativo.");
    printf("\n\tA - Ler os comandos de algum arquivo .txt selecionado.");
    printf("\n\tE - Para SAIR.");
    printf("\n\tEscolha: ");
    scanf("%c", &escolha);
    printf("\n\tSua escolha foi %c", escolha); fflush(stdin);

    switch(escolha)
    {
        case 'i':
        case 'I':
            interativeMode();
            continuar = false;
          break;
        case 'a':
        case 'A':
            readingFileMode();
            continuar = false;
          break;
        case 'e':
        case 'E':
            continuar = false;
          break;
        default:
            printf("\n\tFunção escolhida indisponível. \n\tTecle algo e aperte ENTER para tentar novamente.");
            printf("\n\n\t");
            system("PAUSE");
            system("cls");
            continuar = true;
    }
    }while(continuar);

    return;
}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    /*Loop que ira se encerrar somente quando o usuario teclar 'n' ou 'N'*/
    char continuar;
    do{
        controle();
        do
        {
            system("cls");
            printf("\n\tDeseja continuar (s/n)");
            printf("\n\tSua escolha: ");
            scanf("%c", &continuar); fflush(stdin);
        }while(continuar != 'n' && continuar != 'N' && continuar != 's' && continuar != 'S');
    }while(continuar != 'n' && continuar != 'N');
    /*Loop que ira se encerrar somente quando o usuario teclar 'n' ou 'N'*/

    fclose(inputFile);
    printf("\n\t");
    system("PAUSE");
    return 0;
}
