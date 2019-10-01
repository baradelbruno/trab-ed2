/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 726499 
 * Aluno: Bruno Asti Baradel
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 		31
#define TAM_GENERO 		2
#define TAM_NASCIMENTO 	11
#define TAM_CELULAR 	16
#define TAM_VEICULO 	31
#define TAM_PLACA 		9
#define TAM_DATA 		9
#define TAM_HORA 		6
#define TAM_TRAJETO 	121
#define TAM_VALOR 		7
#define TAM_VAGAS 		2
#define TAM_REGISTRO 	256
#define MAX_REGISTROS 	1000
#define MAX_TRAJETOS 	30
#define TAM_ARQUIVO 	(MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 				"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM 			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO 			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO 				"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO 	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO 					"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 						"FALHA AO REALIZAR OPERACAO!\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; 	/* DD/MM/AAAA */
	char celular[TAM_CELULAR]; 			/*(99) 99999-9999 */
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];				/* AAA-9999 */
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   				/* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   				/* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; 				/* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Ip;

/* Struct para índice secundário de nome */
typedef struct secundary_index
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
} Is;

/* Struct para índice secundário de data */
typedef struct secundary_index_of_date
{
	char pk[TAM_PRIMARY_KEY];
	char data[TAM_DATA];
} Isd;

/* Struct para índice secundário de hora */
typedef struct secundary_index_of_time
{
	char pk[TAM_PRIMARY_KEY];
	char hora[TAM_HORA];
} Ist;

/* Lista ligada usada no índice com lista invertida */
typedef struct linked_list
{
	char pk[TAM_PRIMARY_KEY];
	struct linked_list *prox;
} ll;

/* Struct para lista invertida de trajetos*/
typedef struct reverse_index
{
	char trajeto[TAM_TRAJETO];
	ll *lista;
} Ir;

/* GLOBAL: ARQUIVO DE DADOS */
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Carona */
Carona recuperar_registro(int rrn);

////////////////////////////////
/////////// ÍNDICES ////////////
////////////////////////////////

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int *nregistros);

/* CRIAÇÃO DOS ÍNDICES SECUNDÁRIOS*/
void criar_idriver(Is *idriver, int *nregistros);
void criar_idate(Isd *idate, int *nregistros);
void criar_itime(Ist *itime, int *nregistros);


/* INSERE NO INDICE SECUNDARIO IDRIVER */
void inserirIndiceSecIdriver(int nregistros, Carona *novaCarona, Is *idriver);

/* INSERE NO INDICE SECUNDARIO IDATE */
void inserirIndiceSecIdate(int nregistros, Carona *novaCarona, Isd *idate);

/* INSERE NO INDICE SECUNDARIO ITIME */
void inserirIndiceSecItime(int nregistros, Carona *novaCarona, Ist *itime);

/* INSERE NO INDICE SECUNDARIO IROUTE */
void inserirIndiceSecIroute(int nregistros, Carona *novaCarona, Ir *iroute, int *ntraj);

/* BUSCA POR LOCALIDADE */
void buscarLocalidade(Ip* indice_primario, Ir * iroute, int nregistros, int ntraj);

/* Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

////////////////////////////////
//////// CHAVE PRIMARIA ////////
////////////////////////////////

/* Gera a chave para determinada struct de carona */ 
void gerarChave(Carona *novo);

/* CHECAR SE A CHAVE CRIADA JA EXISTE */
int checarChaveExistente(Carona *novaCarona, Ip* indice_primario, int nregistros);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);

////////////////////////////////
/////////// ARQUIVO ////////////
////////////////////////////////

/* INSERE O REGISTRO NO ARQUIVO */
void inserirNoArquivo(Carona *novaCarona, int *nregistros);

/* EXIBE O IPRIMARY */
void exibir_iprimary(Ip *indice_primario, int nregistros);

/* PREENCHE O ARQUIVO COM # */
void preencherVazio(int tam, char *p);

/* INSERE NO INDICE PRIMARIO */
void inserirIndicePrimario(Carona *carona, Ip *indice_primario, int nregistros);

/* INSERIR NOVO REGISTRO */
void inserir(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int *ntraj, int *nregistros);

/* COMPARA DUAS CHAVES PRIMARIAS */
int comparapk(const void *a, const void *b);

/* BUSCA PELA CHAVE PRIMARIA */
void buscarPk(Ip *indice_primario, int nregistros);

/* LISTAR PELA CHAVE PRIMARIA */
void listarPk(Ip *iprimary, int nregistros);

/* COMPARA PELO NOME DO MOTORISTA */
int comparanome(const void *a, const void *b);

/* BUSCAR PELA DATA */
void buscarData(Ip *indice_primario, Isd *idate, int nregistros);

/* COMPARA PELA DATA */
int comparadata(const void *a, const void *b);

/* LISTA PELO NOME DO MOTORISTA */
void listarDriver(Ip *indice_primario, Is *idriver, int nregistros);

/* COMPARA PELA HORA */
int comparahora(const void* a, const void *b);

/* CRIA E INICIALIZA A LISTA */
void criarLista(Ir *iroute);

/* INSERE NA LISTA */
void inserirNaLista(Carona *novaCarona, ll **lista);

/* COMPARA POR TRAJETOS */
int comparaTrajeto(const void *a, const void *b);

/* REMOVE DA LISTA INVERTIDA */
void removerLista(ll **lista, char *pk);

/* LISTA POR TRAJETO */
void listarTrajeto(Ip* indice_primario, Ir* iroute, int nregistros, int ntraj);

/* LISTAR POR DATA E HORA*/
void listarDataHora(Ip* indice_primario, Isd* idate, Ist* itime, int nregistros);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Verifica se há arquivo de dados */
	int carregarArquivo = 0, nregistros = 0, ntraj = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();


	/* === ALOCAÇÃO E CRIAÇÃO DOS ÍNDICES === */
	
	/* Índice primário */
	Ip *iprimary = (Ip *)malloc(MAX_REGISTROS * sizeof(Ip));
	if (!iprimary)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);
	
	/* <<< COMPLETE AQUI A ALOCAÇÃO E CRIAÇÃO DOS INDICES SECUNDÁRIOS >>> */

    /* INDICE SECUNDARIO DE NOME */
    Is *idriver = (Is *)malloc(MAX_REGISTROS * sizeof(Is));
	if (!idriver)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_idriver(idriver, &nregistros);

    /* INDICE SECUNDARIO DE DATA */
    Isd *idate = (Isd *)malloc(MAX_REGISTROS * sizeof(Isd));
	if (!idate)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_idate(idate, &nregistros);

    /* INDICE SECUNDARIO DE TEMPO */
    Ist *itime = (Ist *)malloc(MAX_REGISTROS * sizeof(Ist));
	if (!itime)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_itime(itime, &nregistros);
    
    /* LISTA INVERTIDA DE TRAJETOS */
    Ir *iroute = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!iroute) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criarLista(iroute);

	/* === ROTINA DE EXECUÇÃO DO PROGRAMA === */
	int opcao = 0;
	while (1)
	{
		//printf("escolha:"); //COMENTEI PARA PASSAR NO CASO 1
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1:
			/* cadastrar */
			inserir(iprimary, idriver, idate, itime, iroute, &ntraj, &nregistros);
			break;

		case 2:
			/* alterar */ 
			printf(INICIO_ALTERACAO);

		/*	if(alterar([args]))
				printf(SUCESSO);
			else
				printf(FALHA); */

			break;

		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);

		/*	if(remover([args]))
				printf(SUCESSO);
			else
				printf(FALHA); */

			break;

		case 4:
			/* buscar */
			printf(INICIO_BUSCA);

			scanf("%d", &opcao);

			switch(opcao){
				case 1:
					buscarPk(iprimary, nregistros);
					break;
				
				case 2:
					buscarData(iprimary, idate, nregistros);
					break;

				case 3:
					buscarLocalidade(iprimary, iroute, nregistros, ntraj);
					break;
				case 4:
					break;
			}
			break;

		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);

			scanf("%d", &opcao);

			switch(opcao){
				case 1:
					listarPk(iprimary, nregistros);
					break;
				case 2:
					listarTrajeto(iprimary, iroute, nregistros, ntraj);
					break;
				case 3:
					listarDriver(iprimary, idriver, nregistros);
					break; 
				case 4: 
					listarDataHora(iprimary, idate, itime, nregistros);
					break;
			}
			break;

		case 6:
			/*	libera espaço */

			/* <<< COLOQUE AQUI A CHAMADA PARA A FUNCAO LIBERAR ESPACO >>> */

			break;

		case 7:
			/* imprime arquivo de dados */
			printf(INICIO_ARQUIVO);

			if (nregistros == 0)
				printf(ARQUIVO_VAZIO);
			else
				printf("%s\n", ARQUIVO);

			break;

		case 8:
			/*	imprime os índices secundários */
			imprimirSecundario(idriver, iroute, idate, itime, nregistros, ntraj);
			break;

		case 9:
			/*	liberar memória e finalizar o programa */
			/* <<< LIBERE A MEMÓRIA DAS ESTRUTURAS CRIADAS >>> */
			return (0);

			break;

		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* ==========================================================================
 * =========================== CÓDIGO DAS FUNÇÕES ===========================
 * ========================================================================== */

void criar_iprimary(Ip *indice_primario, int *nregistros){
	Carona recupera;

	if (nregistros == 0) {
		indice_primario->rrn = 0;
	}
	else
	{
		for (int i = 0; i < *nregistros; i++){
			recupera = recuperar_registro(i);
			for (int j = 0; j < 10; j++){
				indice_primario[i].pk[j] = recupera.pk[j];
			}
		}
	}
}

void criar_idriver(Is *idriver, int *nregistros){
	Carona recupera;

	if (nregistros == 0) {
		return;
	}
	else
	{
		for (int i = 0; i < *nregistros; i++){
			recupera = recuperar_registro(i);
			for (int j = 0; j < 10; j++){
				idriver[i].pk[j] = recupera.pk[j];
			}
			strcpy(idriver[i].nome, recupera.nome);
		}
	}
}

void criar_idate(Isd *idate, int *nregistros){
	Carona recupera;

	if (nregistros == 0) {
		return;
	}
	else{
		for (int i = 0; i < *nregistros; i++){
			recupera = recuperar_registro(i);
			for (int j = 0; j < 10; j++){
				idate[i].pk[j] = recupera.pk[j];
			}
			strcpy(idate[i].data, recupera.data);
		}
	}
}

void criar_itime(Ist *itime, int *nregistros){
	Carona recupera;

	if (nregistros == 0) {
		return;
	}
	else{
		for (int i = 0; i < *nregistros; i++){
			recupera = recuperar_registro(i);
			for (int j = 0; j < 10; j++){
				itime[i].pk[j] = recupera.pk[j];
			}
			strcpy(itime[i].hora, recupera.hora);
		}
	}
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{

	if (rrn < 0)
		return 0;
	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.genero);
	printf("%s\n", j.nascimento);
	printf("%s\n", j.celular);
	printf("%s\n", j.veiculo);
	printf("%s\n", j.placa);
	printf("%s\n", j.data);
	printf("%s\n", j.hora);
	printf("%s\n", j.valor);
	printf("%s\n", j.vagas);

	strcpy(trajeto, j.trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL)
	{
		printf("%s", traj);
		traj = strtok(NULL, "|");
		if (traj != NULL)
		{
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

/* carrega dados do arquivo e retorna
 * a quantidade de registros lidos */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Carona */
Carona recuperar_registro(int rrn)
{
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[256] = '\0';
	Carona j;


	p = strtok(temp, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.genero, p);
	p = strtok(NULL, "@");
	strcpy(j.nascimento, p);
	p = strtok(NULL, "@");
	strcpy(j.celular, p);
	p = strtok(NULL, "@");
	strcpy(j.veiculo, p);
	p = strtok(NULL, "@");
	strcpy(j.placa, p);
	p = strtok(NULL, "@");
	strcpy(j.trajeto, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.hora, p);
	p = strtok(NULL, "@");
	strcpy(j.valor, p);
	p = strtok(NULL, "@");
	strcpy(j.vagas, p);

	gerarChave(&j);
	return j;
}

/* GERA A CHAVE */
void gerarChave(Carona *novo){
    novo->pk[0] = novo->nome[0];
    novo->pk[1] = novo->placa[0];
    novo->pk[2] = novo->placa[1];
    novo->pk[3] = novo->placa[2];
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    novo->pk[8] = novo->hora[0];
    novo->pk[9] = novo->hora[1];
    novo->pk[10] = '\0'; 
}

/* ==========================================================================
 * ============================= CRIAR ÍNDICES ==============================
 * ========================================================================== */

/* Imprimir indices secundarios */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj)
{
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if (!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint)
	{
	case 1:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idriver[i].pk, idriver[i].nome);
		}
		break;
	case 2:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idate[i].pk, idate[i].data);
		}
		break;
	case 3:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", itime[i].pk, itime[i].hora);
		}
		break;
	case 4:
		for (int i = 0; i < ntraj; i++)
		{
			printf("%s", iroute[i].trajeto);
			aux = iroute[i].lista;
			while (aux != NULL)
			{
				printf(" %s", aux->pk);
				aux = aux->prox;
			}
			printf("\n");
		}
		break;
	default:
		printf(OPCAO_INVALIDA);
		break;
	}
}

/* INSERE UM NOVO REGISTRO (QUANDO NÃO HÁ ARQUIVO DE ENTRADA) */
void inserir(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int *ntraj, int *nregistros)
{
    Carona novaCarona;

    scanf("\n%[^\n]%*c", novaCarona.nome);
    scanf("\n%[^\n]%*c", novaCarona.genero);
   	scanf("\n%[^\n]%*c", novaCarona.nascimento);
    scanf("\n%[^\n]%*c", novaCarona.celular);
    scanf("\n%[^\n]%*c", novaCarona.veiculo);
    scanf("\n%[^\n]%*c", novaCarona.placa);
    scanf("\n%[^\n]%*c", novaCarona.trajeto);
    scanf("\n%[^\n]%*c", novaCarona.data);
    scanf("\n%[^\n]%*c", novaCarona.hora);
    scanf("\n%[^\n]%*c", novaCarona.valor);
    scanf("\n%[^\n]%*c", novaCarona.vagas);


    gerarChave(&novaCarona);
    if(checarChaveExistente(&novaCarona, iprimary, *nregistros)){
        inserirNoArquivo(&novaCarona, nregistros);
        inserirIndicePrimario(&novaCarona, iprimary, *nregistros);
		inserirIndiceSecIdriver(*nregistros, &novaCarona, idriver);
        inserirIndiceSecIdate(*nregistros, &novaCarona, idate);
        inserirIndiceSecItime(*nregistros, &novaCarona, itime);
		inserirIndiceSecIroute(*nregistros, &novaCarona, iroute, ntraj);

        (*nregistros)++;
    }
	else
		printf(ERRO_PK_REPETIDA, novaCarona.pk);

	//printf("Valor de nregistros: %d\n", *nregistros);
}

void inserirNoArquivo(Carona *novaCarona, int *nregistros){
    int tam;

    char *p = ARQUIVO + (256 * (*nregistros));

    /* SEM VALIDAÇÃO DE TAMANHO */
    sprintf(p, "%s@", novaCarona->nome);
    tam = strlen(novaCarona->nome) + 1;
    sprintf(p+tam, "%s@", novaCarona->genero);
    tam += strlen(novaCarona->genero) + 1;
    sprintf(p+tam, "%s@", novaCarona->nascimento);
    tam += strlen(novaCarona->nascimento) + 1;
    sprintf(p+tam, "%s@", novaCarona->celular);
    tam += strlen(novaCarona->celular) + 1;
    sprintf(p+tam, "%s@", novaCarona->veiculo);
    tam += strlen(novaCarona->veiculo) + 1;
    sprintf(p+tam, "%s@", novaCarona->placa);
    tam += strlen(novaCarona->placa) + 1;
    sprintf(p+tam, "%s@", novaCarona->trajeto);
    tam += strlen(novaCarona->trajeto) + 1;
    sprintf(p+tam, "%s@", novaCarona->data);
    tam += strlen(novaCarona->data) + 1;
    sprintf(p+tam, "%s@", novaCarona->hora);
    tam += strlen(novaCarona->hora) + 1;
    sprintf(p+tam, "%s@", novaCarona->valor);
    tam += strlen(novaCarona->valor) + 1;
    sprintf(p+tam, "%s@", novaCarona->vagas);
    tam += strlen(novaCarona->vagas) + 1;

    preencherVazio(tam, p);
}

int checarChaveExistente(Carona *novaCarona, Ip* indice_primario, int nregistros){
	Ip *a;

	a = (Ip*) bsearch(novaCarona->pk, indice_primario, nregistros, sizeof(Ip), comparapk);

   	return (a == NULL);
}

void preencherVazio(int tam, char *p){
    for(;  tam < 256; tam++)
        sprintf(p+tam,"#");
}

void inserirIndicePrimario(Carona *carona, Ip *indice_primario, int nregistros){
	for (int i=0; i < 10; i++){
		indice_primario[nregistros].pk[i] = carona->pk[i];
	}
	indice_primario[nregistros].rrn = nregistros;

	qsort(indice_primario, nregistros+1, sizeof(Ip), comparapk);
}

int comparapk(const void *a, const void *b){
	const Ip *x = a;
	const Ip *y = b;

	return strcmp(x->pk , y->pk);
}

void inserirIndiceSecIdriver(int nregistros, Carona *novaCarona, Is *idriver){
	for (int i=0; i < 10; i++){
		idriver[nregistros].pk[i] = novaCarona->pk[i];
	}
	strcpy(idriver[nregistros].nome, novaCarona->nome);

	qsort(idriver, nregistros+1, sizeof(Is), comparanome);
	
	//SE OS NOMES FOREM IGUAIS, ORDENAR PELA CHAVE!!!
}

void inserirIndiceSecIdate(int nregistros, Carona *novaCarona, Isd *idate){
	Isd y;
	//printf("ENTROU NA INSERIR INDICE SEC IDATE\n");
	for (int i=0; i < 10; i++){
		idate[nregistros].pk[i] = novaCarona->pk[i];
	}
	 strcpy(idate[nregistros].data, novaCarona->data);

	qsort(idate, nregistros+1, sizeof(Isd), comparadata);
	//SE AS DATAS FOREM IGUAIS, ORDENAR PELA CHAVE
}

void inserirIndiceSecItime(int nregistros, Carona *novaCarona, Ist *itime){
	Isd y;
	for (int i=0; i < 10; i++){
		itime[nregistros].pk[i] = novaCarona->pk[i];
	}
 	strcpy(itime[nregistros].hora, novaCarona->hora);

	qsort(itime, nregistros+1, sizeof(Ist), comparahora);
	//SE AS DATAS FOREM IGUAIS, ORDENAR PELA CHAVE
}

int comparanome(const void *a, const void *b){
	const Is *x = a;
	const Is *y = b;

	return strcmp(x->nome, y->nome);
}

void exibir_iprimary(Ip *indice_primario, int nregistros) {
	for (int i = 0; i < nregistros; i++){
		printf("%s %d\n", indice_primario->pk, indice_primario->rrn);
	}
}

void buscarPk(Ip *indice_primario, int nregistros){
	char pk[TAM_PRIMARY_KEY];

	scanf("\n%[^\n]%*c", pk);

	Ip *aux;
	aux = (Ip *) bsearch(pk, indice_primario, nregistros, sizeof(Ip) , comparapk);

	if(aux == NULL || aux->rrn == -1)
		printf(REGISTRO_N_ENCONTRADO);

	else
		exibir_registro(aux->rrn);
}

void buscarData(Ip *indice_primario, Isd *idate, int nregistros){
	Ip *aux_rrn;
	Isd *aux;

	aux = (Isd *) calloc (9,sizeof(char));

	getchar();scanf("%[^\n]s", aux->data);
	//printf("%s", aux->data);
		
	aux = (Isd *) bsearch(aux, idate, nregistros, sizeof(Isd) , comparadata);

	if(aux == NULL)
		printf(REGISTRO_N_ENCONTRADO);
	else{
		 for (int i = 0; i < nregistros; i++){
		 	if (strcmp(aux->pk, idate[i].pk) == 0){
		 		aux_rrn = (Ip *) bsearch(idate[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk);
		 		if (aux_rrn->rrn != -1){
		 			exibir_registro(aux_rrn->rrn);
		 		}
		 		else
		 			printf(REGISTRO_N_ENCONTRADO);
		 	}
		}
	}
}

int comparadata(const void *a, const void *b){
	const Isd *x = (Isd *)a;
	const Isd *y = (Isd *)b;
	int i, a_dia = 0, b_dia = 0, a_mes = 0, b_mes = 0, a_ano = 0, b_ano = 0;
	
	for (i = 0; i < 2; i++){
		a_dia = 10 * a_dia + (x->data[i] - '0');
		b_dia = 10 * b_dia + (y->data[i] - '0');
	}
	for (i = 3; i < 5; i++){
		a_mes = 10 * a_mes + (x->data[i] - '0');
		b_mes = 10 * b_mes + (y->data[i] - '0');
	}
	for (i = 6; i < 8; i++){
		a_ano = 10 * a_ano + (x->data[i] - '0');
		b_ano = 10 * b_ano + (y->data[i] - '0');
	}

	if(a_ano < b_ano)
		return -1;
	else if (a_ano == b_ano && a_mes < b_mes)
		return -1;
	else if(a_ano == b_ano && a_mes == b_mes && a_dia < b_dia)
		return -1;

	else if(a_ano == b_ano && a_mes == b_mes && a_dia == b_dia)
		return 0;
	else
		return 1;
}

void listarPk(Ip *iprimary, int nregistros){    
    for(int i = 0; i < nregistros; i++)
    {
        if(exibir_registro(iprimary[i].rrn))
            if(i < nregistros - 1)
                printf("\n");
    }
}

void listarDriver(Ip *indice_primario, Is *idriver, int nregistros){
	Ip *aux;
	
	for(int i = 0; i < nregistros; i++)
    {
		aux = (Ip *) bsearch(idriver[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk);
        if(exibir_registro(indice_primario[i].rrn))
            if(i < nregistros - 1)
                printf("\n");
    }
}

int comparahora(const void* a, const void *b){
	const Ist *x = (Ist *)a;
	const Ist *y = (Ist *)b;
	int i, a_hora = 0, b_hora = 0, a_min = 0, b_min = 0;
	
	for (i = 0; i < 2; i++){
		a_hora = 10 * a_hora + (x->hora[i] - '0');
		b_hora = 10 * b_hora + (y->hora[i] - '0');
	}
	for (i = 3; i < 5; i++){
		a_min = 10 * a_min + (x->hora[i] - '0');
		b_min = 10 * b_min + (y->hora[i] - '0');
	}

	if(a_hora < b_hora)
		return -1;
	else if (a_hora == b_hora && a_min < b_min)
		return -1;
	else if(a_hora == b_hora && a_min == b_min)
		return 0;
	else
		return 1;
}

void inserirNaLista(Carona *novaCarona, ll **lista){
	ll *node = (ll *) malloc(sizeof(ll));

	strcpy(node->pk, novaCarona->pk);

	/* LISTA VAZIA */
	if (*lista == NULL){
		*lista = node;
		node->prox = NULL;
		return;
	}

	/* INSERIR NO INICIO */
	else if(strcmp((*lista)->pk, node->pk) > 0){
		node->prox = (*lista);
		*lista = node;
		return;
	}

	/* INSERIR NO MEIO */
	else{

		ll *aux = *lista;

		while(aux->prox != NULL){
			if(strcmp(aux->prox->pk, node->pk) > 0){
				node->prox = aux->prox;
				aux->prox = node;
				return;
			}
			aux = aux->prox;
		}

		/* INSERIR NO FINAL */
		node->prox = NULL;
		aux->prox = node;
		return;
	}
}

void criarLista(Ir *iroute){
	
	//CONDIÇÃO PARA CRIAR COM ARQUIVO
	
	for(int i = 0; i < MAX_REGISTROS; i++)
		iroute[i].lista = NULL;
}

void inserirIndiceSecIroute(int nregistros, Carona *novaCarona, Ir *iroute, int *ntraj){
	char* traj, trajeto[TAM_TRAJETO];
	int i = 0;
	Ir *aux;

	aux = (Ir *) malloc (sizeof(char));

	strcpy(trajeto, novaCarona->trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL){
		aux = (Ir *) bsearch(traj, iroute, *ntraj, sizeof(Ir) , comparaTrajeto);
		
		if(aux == NULL){
			strcpy(iroute[*ntraj].trajeto, traj);
			inserirNaLista(novaCarona, &iroute[*ntraj].lista);
			(*ntraj)++;
			qsort(iroute, *ntraj ,sizeof(Ir) , comparaTrajeto);
		}
		else{
			for(int i=0; i < (*ntraj); i++){
				if(strcmp(iroute[i].trajeto, aux->trajeto) == 0)
					inserirNaLista(novaCarona, &iroute[i].lista);
			}
		}
		traj = strtok(NULL, "|");
	}		
}

int comparaTrajeto(const void *a, const void *b){
	const Ir *x = a;
 	const Ir *y = b;

  return strcmp(x->trajeto , y->trajeto);
}

void buscarLocalidade(Ip* indice_primario, Ir * iroute, int nregistros, int ntraj){
	Ip *aux_rrn;
	Ir *copia, *aux2, *string;
	int flag[ntraj], primeira = 1;

	for(int i = 0; i < ntraj; i++)
		flag[i] = 0;

	aux2 = (Ir *) malloc (sizeof(char));
	string = (Ir *) malloc (sizeof(char));
	copia = (Ir *) malloc (sizeof(char));

	getchar();scanf("%[^\n]s", string->trajeto);
	
	aux2 = (Ir *) bsearch(string->trajeto, iroute, ntraj, sizeof(Ir) , comparaTrajeto);
	if(aux2 == NULL){
		printf(REGISTRO_N_ENCONTRADO);
	}
	else{
		memcpy(copia, aux2, sizeof(Ir));
		while(copia->lista != NULL){
			//printf("aux2->lista->pk: %s\n", aux2->lista->pk);
			aux_rrn = (Ip *) bsearch(copia->lista->pk, indice_primario, nregistros, sizeof(Ip), comparapk);
			
			if (aux_rrn->rrn != -1){
				exibir_registro(aux_rrn->rrn);

				if(copia->lista->prox != NULL)
					printf("\n");

				//removerLista(&aux2->lista, aux2->lista->pk);
			}
			else
				printf(REGISTRO_N_ENCONTRADO);

			copia->lista = copia->lista->prox;
		}
	}
}

void removerLista(ll **lista, char *pk){
	ll *a =  *lista;
	ll* x;

	//só tem um registro
	if(a->prox == NULL){
		if(strcmp(a->pk, pk) == 0){
			*lista = NULL;
			return;
		}
		else
			return;
	}

	//remover no inicio
    if(strcmp(a->pk, pk) == 0){
       *lista = a->prox;
       free(a);
       return;
    }

	while(a->prox->prox != NULL){
	    //remover do meio
		if(strcmp(a->prox->pk, pk) == 0){
			a->prox = a->prox->prox;
			return;
		}
		a = a->prox;
	}

	
	if(a->prox->prox == NULL){
		if(strcmp(a->prox->pk, pk) == 0){
			//remover do fim
			a->prox = NULL;
			return;
		}
	}
	return;
}

void listarTrajeto(Ip* indice_primario, Ir* iroute, int nregistros, int ntraj){
	Ip *aux_rrn;
	Ir *copia;
	int flag[nregistros], primeira = 1;

	for(int i = 0; i < nregistros; i++)
		flag[i] = 0;

	copia = (Ir *) malloc (sizeof(char));
	
	for (int j = 0; j < ntraj; j++){
		copia[j].lista = iroute[j].lista;
		
		while(copia[j].lista != NULL){
			aux_rrn = (Ip *) bsearch(copia[j].lista->pk, indice_primario, nregistros, sizeof(Ip), comparapk);

			if (aux_rrn->rrn != -1){
				exibir_registro(aux_rrn->rrn);

				if(copia[j].lista->prox != NULL)
					printf("\n");

				//removerLista(&copia[j].lista, copia[j].lista->pk);
			}
			else
				printf(REGISTRO_N_ENCONTRADO);

			copia[j].lista = copia[j].lista->prox;
		}
	}
}

void listarDataHora(Ip* indice_primario, Isd* idate, Ist* itime, int nregistros){
	Ip *aux_rrn;
	int j = 0, k = 0, primeira = 1;
	int flag[nregistros];

	for(int i = 0; i < nregistros; i++)
		flag[i] = 0;

	for (int i = 0; i < nregistros; i++){
		aux_rrn = (Ip *) bsearch(idate[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk);

		if(idate[i+1].data != NULL)
			if(strcmp(idate[i].data, idate[i+1].data) == 0)
				for (int j = i; j < nregistros; j++){
					aux_rrn = (Ip *) bsearch(itime[j].pk, indice_primario, nregistros, sizeof(Ip), comparapk);
					if(aux_rrn != NULL)
						exibir_registro(aux_rrn->rrn);
				}
		if(aux_rrn != NULL)
			exibir_registro(aux_rrn->rrn);
		else
			printf(REGISTRO_N_ENCONTRADO);

		if(i < nregistros-1)
			printf("\n");
	}
}