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

/*struct para vetor de strings */
typedef struct stringvector
{
	char string[TAM_NOME];
} sv;

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
void criar_iroute(Ir* iroute, int *nregistros, int *ntraj);


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

/* BUSCAR PELA LOCALIDADE E DATA */
void buscarLocalidadeData(Ip* indice_primario, Ir* iroute, Isd* idate, Ist* itime, int ntraj, int nregistros);

/* LISTAR POR LOCALIDADE, DATA E HORA */
void listarLocalidadeDataHora(Ip* indice_primario, Ir* iroute, Isd* idate, Ist* itime, int ntraj, int nregistros, int nRemovidos);

/* COMPARA PK ENTRE IP E ISD */
int comparapk2(const void *a, const void *b);

/* ALTERAR O NUMERO DE VAGAS */
int alterar(Ip* indice_primario, int nregistros);

/* FUNÇÃO AUXILIAR PARA A ALTERAÇÃO DAS VAGAS */
int checaNovaVaga(char *vagas);

/* REMOVER */
int remover(Ip *indice_primario, int nregistros, int *nRemovidos);

/* COMPARAR CADA UM DOS NOMES DO MOTORISTA */
int comparaNomeaNome(Is x, Is y);

/* LIBERAR ESPAÇO NO ARQUIVO */
void liberarEspaco(int *nregistros, int *ntraj, Ip* indice_primario, Ir* iroute, Is* idriver, Isd* idate, Ist* itime);

/* RECRIAR OS INDICES APÓS LIBERAR MEMÓRIA */
void recriaIndices(Ip* indice_primario, Is* idriver, Isd* idate, Ist* itime, int * nregistros, Ir* iroute, int *ntraj);

/* LIBERA A MEMÓRIA DE IROUTE */
void liberar_iroute(Ir* iroute, int *ntraj);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Verifica se há arquivo de dados */
	int carregarArquivo = 0, nregistros = 0, ntraj = 0, nRemovidos = 0;
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
	criar_iroute(iroute, &nregistros, &ntraj);

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

			if(alterar(iprimary, nregistros))
				printf(SUCESSO);
			else
				printf(FALHA);

			break;

		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);

			if(remover(iprimary, nregistros, &nRemovidos))
				printf(SUCESSO);
			else
				printf(FALHA); 

			break;

		case 4:
			/* buscar */
			printf(INICIO_BUSCA);

			scanf("%d", &opcao);

			switch(opcao){
				case 1:
					printf("BUSCAR PK\n");
					buscarPk(iprimary, nregistros);
					break;
				
				case 2:
					printf("BUSCAR DATA\n");
					buscarData(iprimary, idate, nregistros);
					break;

				case 3:
					printf("BUSCAR LOCALIDADE\n");
					buscarLocalidade(iprimary, iroute, nregistros, ntraj);
					break;
				case 4:
					printf("BUSCAR LOCALIDADE DATA\n");
					buscarLocalidadeData(iprimary, iroute, idate, itime, ntraj, nregistros);
					break;
			}
			break;

		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);

			scanf("%d", &opcao);

			switch(opcao){
				case 1:
					printf("LISTAR PK\n");
					listarPk(iprimary, nregistros);
					break;
				case 2:
					printf("LISTAR TRAJETO\n");
					listarTrajeto(iprimary, iroute, nregistros, ntraj);
					break;
				case 3:
					printf("LISTAR DRIVER\n");
					listarDriver(iprimary, idriver, nregistros);
					break; 
				case 4: 
					printf("LISTAR DATA E HORA\n");
					listarDataHora(iprimary, idate, itime, nregistros);
					break;
				case 5:
					printf("LISTAR LOCALIDADE DATA HORA\n");
					listarLocalidadeDataHora(iprimary, iroute, idate, itime, ntraj, nregistros, nRemovidos);
				break;
			}
			break;

		case 6:
			/*	libera espaço */

			liberarEspaco(&nregistros, &ntraj, iprimary, iroute, idriver, idate, itime);
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
			strcpy(indice_primario[i].pk, recupera.pk);
			indice_primario[i].rrn = i;
		}
	}
	qsort(indice_primario, *nregistros, sizeof(Ip), comparapk);
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
			strcpy(idriver[i].pk, recupera.pk);
			strcpy(idriver[i].nome, recupera.nome);
		}
		qsort(idriver, (*nregistros), sizeof(Is), comparanome);
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
			strcpy(idate[i].pk, recupera.pk);
			strcpy(idate[i].data, recupera.data);
		}
		qsort(idate, (*nregistros), sizeof(Isd), comparadata);
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
			strcpy(itime[i].pk, recupera.pk);
			strcpy(itime[i].hora, recupera.hora);
		}
		qsort(itime, (*nregistros), sizeof(Ist), comparahora);
	}
}

void criar_iroute(Ir* iroute, int *nregistros, int *ntraj){
	Carona recupera;
	
	// printf("PODEPAAA\n");
	// criarLista(iroute);
	// printf("PODEPA2\n");

	if(*nregistros == 0)
		return;

	else{
		for(int i=0; i< *nregistros; i++){
			recupera = recuperar_registro(i);
			//printf("VAI INSERIR NO IROUTE %d\n", i);
			inserirIndiceSecIroute(*nregistros, &recupera, iroute, ntraj);
			//printf("INSERIU IROUTE\n");
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
	Ip *a;
	char* p;

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

	int aux = checarChaveExistente(&novaCarona, iprimary, *nregistros);
    if(aux != 0){
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

	a = (Ip*) malloc(sizeof(Ip));

	a = (Ip*) bsearch(novaCarona->pk, indice_primario, nregistros, sizeof(Ip), comparapk);
	if(a == NULL)
		return 1;
	else if(a->rrn == -1)
		return 2;
	else
		return 0;
}

void preencherVazio(int tam, char *p){
    for(;  tam < 256; tam++)
        sprintf(p+tam,"#");
}

void inserirIndicePrimario(Carona *carona, Ip *indice_primario, int nregistros){
	strcpy(indice_primario[nregistros].pk, carona->pk);
	indice_primario[nregistros].rrn = nregistros;

	qsort(indice_primario, nregistros+1, sizeof(Ip), comparapk);
}

int comparapk(const void *a, const void *b){
	const Ip *x = a;
	const Ip *y = b;

	return strcmp(x->pk , y->pk);
}

int comparapk2(const void *a, const void *b){
	char x[11];
	strcpy(x, a);
	const Isd *y = b;

	return strcmp(x, y->pk);
}

void inserirIndiceSecIdriver(int nregistros, Carona *novaCarona, Is *idriver){
	strcpy(idriver[nregistros].pk, novaCarona->pk);
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
	Is *x = (Is* )a;
	Is *y = (Is*) b;
	int t;
	t = comparaNomeaNome(*x, *y);
	if(t == 0)
		return strcmp(x->pk, y->pk);
	else
		return t;
}

int comparaNomeaNome(Is x, Is y){
	char *traj, *traj2, copiax[TAM_NOME], copiay[TAM_NOME];
	int i = 0, j = 0, contador;
	sv *vetor, *vetor2;

	vetor = (sv *) malloc(sizeof(sv)*10);
	vetor2 = (sv *) malloc(sizeof(sv)*10);

	strcpy(copiax, x.nome);
	strcpy(copiay, y.nome);

	traj = strtok(copiax, " ");

	while(traj != NULL){
		strcpy(vetor[i].string, traj);
		//printf("%s\n", vetor[i].string);
		i++;
		traj = strtok(NULL, " \n\t");
	}
	traj2 = strtok(copiay, " ");

	while(traj2!= NULL){
		strcpy(vetor2[j].string, traj2);
		j++;
		traj2 = strtok(NULL, " \n\t");
	}
	if(i > j)
		contador = i;
	else 
		contador = j;

	for(int k = 0; k < contador; k++){
		//printf("VETOR[K]: %s\nVETOR2[K]: %s\n", vetor[k].string, vetor2[k].string);
		if(strcmp(vetor[k].string, vetor2[k].string) < 0){
			free(vetor);
			free(vetor2);
			return -1;
		}
		else if(strcmp(vetor[k].string, vetor2[k].string) >= 1){
			free(vetor);
			free(vetor2);
			return 1;
		}
	}
	free(vetor);
	free(vetor2);
	return 0;
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
	aux = (Ip *) bsearch(pk, indice_primario, nregistros, sizeof(Ip) , comparapk2);

	if(aux == NULL || aux->rrn == -1)
		printf(REGISTRO_N_ENCONTRADO);

	else
		exibir_registro(aux->rrn);
}

void buscarData(Ip *indice_primario, Isd *idate, int nregistros){
	Ip *aux_rrn;
	Isd *aux;
	int flag = 0;

	aux = (Isd *) calloc (9,sizeof(char));

	getchar();scanf("%[^\n]s", aux->data);
	//printf("%s", aux->data);
		
	aux = (Isd *) bsearch(aux, idate, nregistros, sizeof(Isd) , comparadata);

	if(aux == NULL)
		printf(REGISTRO_N_ENCONTRADO);
	else{
		 for (int i = 0; i < nregistros; i++){
		 	if (strcmp(aux->pk, idate[i].pk) == 0){
		 		aux_rrn = (Ip *) bsearch(idate[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
		 		if (aux_rrn != NULL && aux_rrn->rrn != -1){
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
    
	if(nregistros == 0)
		printf(REGISTRO_N_ENCONTRADO);
	else{
		for(int i = 0; i < nregistros; i++)
		{
			if(exibir_registro(iprimary[i].rrn))
				if(i < nregistros - 1)
					printf("\n");
		}
	}
}

void listarDriver(Ip *indice_primario, Is *idriver, int nregistros){
	Ip *aux;
	
	for(int i = 0; i < nregistros; i++){
		aux = (Ip *) bsearch(idriver[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
		if(aux != NULL){
			exibir_registro(aux->rrn);
			if(i < nregistros - 1 && aux->rrn != -1)
				printf("\n");
		}
		else
			printf(REGISTRO_N_ENCONTRADO);
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
	ll *node = (ll *) calloc(1, sizeof(ll));
	ll *aux = (ll *) calloc(1, sizeof(ll));

	strcpy(node->pk, novaCarona->pk);

	//printf("hmm\n");
	/* LISTA VAZIA */
	if (*lista == NULL){
		*lista = node;
		node->prox = NULL;
		//printf("pk: %s\n", node->pk);
		return;
	}

	/* INSERIR NO INICIO */
	else if(strcmp((*lista)->pk, node->pk) > 0){
		//printf("1\n");
		node->prox = (*lista);
		*lista = node;
		return;
	}

	/* INSERIR NO MEIO */
	else{
		aux = *lista;
	//printf("salve salve\n");
		while(aux->prox != NULL){
			//printf("AUX PK: %s AUX PROX PK: %s\n", aux->pk, aux->prox->pk);
			if(strcmp(aux->prox->pk, node->pk) > 0){
				//printf("2\n");
				node->prox = aux->prox;
				aux->prox = node;
				return;
			}
			aux = aux->prox;
		}

		/* INSERIR NO FINAL */
		//printf("3\n");
		node->prox = NULL;
		aux->prox = node;
		return;
	}
}

void criarLista(Ir *iroute){
	
	for(int i = 0; i < MAX_REGISTROS; i++){
		ll *lista2 = (ll *) malloc (MAX_REGISTROS * sizeof(ll));
  		if (!lista2) {
			perror(MEMORIA_INSUFICIENTE);
			exit(1);
		lista2 = NULL;
		}
		iroute[i].lista = lista2;
	}
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
			//printf("VAI INSERIR NA LISTA IROUTE: %s\n", iroute[*ntraj].trajeto);
			inserirNaLista(novaCarona, &iroute[*ntraj].lista);
			//printf("INSERIU NA LISTA IROUTE\n");
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
	int flag[ntraj], primeira = 1, flag2 = 0, flag3 = 0;

	for(int i = 0; i < ntraj; i++)
		flag[i] = 0;

	aux_rrn = (Ip *) calloc(1, sizeof(Ip));

	string = (Ir *) calloc (MAX_REGISTROS, sizeof(Ir));
	copia = (Ir *) calloc (MAX_REGISTROS, sizeof(Ir));

	getchar();scanf("%[^\n]s", string->trajeto);
	//printf("ntraj: %d\n", ntraj);
	//printf("string->trajeto: %s\n", string->trajeto);

	aux2 = (Ir *) bsearch(string->trajeto, iroute, ntraj, sizeof(Ir) , comparaTrajeto);
	if(aux2 == NULL){
		flag2 = 1;
	}
	else{
		memcpy(copia, aux2, sizeof(Ir));
		while(copia->lista != NULL){
			for(int i = 0; i<nregistros; i++){
				//printf("a\n");
				if(strcmp(copia->lista->pk, indice_primario[i].pk) == 0){
					//printf("ENTROU\n");
					//printf("pkk: %s\n", indice_primario[i].pk);
					strcpy(aux_rrn->pk, indice_primario[i].pk);
					//printf("hmm\n");
					aux_rrn->rrn = indice_primario[i].rrn;
				}
			}
				//printf("olha: %d a: %s\n", indice_primario[i].rrn, indice_primario[i].pk);
			//printf("copia pk: %s\n", copia->lista->pk);

			//aux_rrn = (Ip *) bsearch(copia->lista->pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
			//printf("rrn: %d pk: %s\n", aux_rrn->rrn, aux_rrn->pk);
			if (aux_rrn != NULL){
				exibir_registro(aux_rrn->rrn);
				flag3 = 1;

				if(copia->lista->prox != NULL)
					printf("\n");
			}
			else
				flag2 = 1;

			copia->lista = copia->lista->prox;
		}
	}
	if(flag2 == 1 && flag3 == 0)
		printf(REGISTRO_N_ENCONTRADO);
	// free(string);
	// free(copia);
}

void removerLista(ll **lista, char *pk){
	ll *a = *lista;
	ll* x;
	a = *lista;
	//("%s\n", a->prox->pk);
	//só tem um registro
	if(a->prox == NULL){
		//printf("1\n");
		if(strcmp(a->pk, pk) == 0){
			*lista = NULL;
			return;
		}
		else{
			return;
		}
	}

	//remover no inicio
    if(strcmp(a->pk, pk) == 0){
		//printf("2\n");
       *lista = a->prox;
       free(a);
       return;
    }

	while(a->prox->prox != NULL){
	    //remover do meio
		if(strcmp(a->prox->pk, pk) == 0){
			//printf("3\n");
			a->prox = a->prox->prox;
			return;
		}
		a = a->prox;
	}

	
	if(a->prox->prox == NULL){
		if(strcmp(a->prox->pk, pk) == 0){
			//printf("4\n");
			//remover do fim
			a->prox = NULL;
			return;
		}
	}
	//printf("VISH MARIA\n");
	return;
}

void listarTrajeto(Ip* indice_primario, Ir* iroute, int nregistros, int ntraj){
	Ip *aux_rrn;
	Ir *copia;
	int flag[nregistros], primeira = 1, k = 0, t = 0;
	for(int i = 0; i < nregistros; i++)
		flag[i] = 0;

	copia = (Ir *) malloc (sizeof(char));

	for(int i = 0; i < ntraj; i++){
		copia[i].lista = iroute[i].lista;
		while(copia[i].lista != NULL){
			k++;
			copia[i].lista = copia[i].lista->prox;
		}
	}

	for (int j = 0; j < ntraj; j++){
		copia[j].lista = iroute[j].lista;
		
		while(copia[j].lista != NULL){
			aux_rrn = (Ip *) bsearch(copia[j].lista->pk, indice_primario, nregistros, sizeof(Ip), comparapk2);

			if (aux_rrn != NULL){
				exibir_registro(aux_rrn->rrn);
			
			    t++;

			    if(t <= k-1)
				    printf("\n");
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

	if(nregistros == 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}

	for(int i = 0; i < nregistros; i++)
		flag[i] = 0;

	for (int i = 0; i < nregistros; i++){
		aux_rrn = (Ip *) bsearch(idate[i].pk, indice_primario, nregistros, sizeof(Ip), comparapk2);

		if(idate[i+1].data != NULL)
			if(strcmp(idate[i].data, idate[i+1].data) == 0)
				for (int j = i; j < nregistros; j++){
					aux_rrn = (Ip *) bsearch(itime[j].pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
					
					if(aux_rrn != NULL){
						exibir_registro(aux_rrn->rrn);
					}
					else
						printf(REGISTRO_N_ENCONTRADO);
					
				}
		if(aux_rrn != NULL)
			exibir_registro(aux_rrn->rrn);
		else
			printf(REGISTRO_N_ENCONTRADO);

		if(i < nregistros-1)
			printf("\n");
	}
}

void buscarLocalidadeData(Ip* indice_primario, Ir* iroute, Isd* idate, Ist* itime, int ntraj, int nregistros){
	Ip *aux_rrn;
	Isd* data, *aux_data;
	Ir *copia, *aux2, *string;
	int flag[ntraj], primeira = 1, ultima = 0, i = 0, flag2 = 0, flag3 = 0;

	for(int i = 0; i < ntraj; i++)
		flag[i] = 0;

	data = (Isd *) malloc(sizeof(char));
	aux_data = (Isd *) malloc(sizeof(char));
	aux2 = (Ir *) malloc (sizeof(Ir));
	string = (Ir *) malloc (sizeof(char));
	copia = (Ir *) malloc (sizeof(Ir));

	getchar();scanf("%[^\n]s", string->trajeto);
	getchar();scanf("%[^\n]s", data->data);
	

	aux2 = (Ir *) bsearch(string->trajeto, iroute, ntraj, sizeof(Ir) , comparaTrajeto);
	if(aux2 == NULL){
		flag2 = 1;
	}
	else{
		aux_data = (Isd *) bsearch(data, idate, nregistros, sizeof(Isd) , comparadata);
		if(aux_data == NULL){
			flag2 = 1;
		}
		else{
			memcpy(copia, aux2, sizeof(Ir));
			while(copia->lista != NULL){
				i++;
				copia->lista = copia->lista->prox;
			}
			memcpy(copia, aux2, sizeof(Ir));
			while(copia->lista != NULL){
				if(strcmp(copia->lista->pk, aux_data->pk) == 0){
					aux_rrn = (Ip *) bsearch(copia->lista->pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
				
					if (aux_rrn != NULL && aux_rrn->rrn != -1){
						ultima++;
						flag3 = 1;
						exibir_registro(aux_rrn->rrn);

						if(ultima <= i-5)
							printf("\n");
					}
					else{
						flag2 = 1;
					}
				}
				copia->lista = copia->lista->prox;
			}
			if(flag3 == 0)
				printf(REGISTRO_N_ENCONTRADO);
		}
	}
	if(flag2 == 1 && flag3 == 0)
		printf(REGISTRO_N_ENCONTRADO);
}

void listarLocalidadeDataHora(Ip* indice_primario, Ir* iroute, Isd* idate, Ist* itime, int ntraj, int nregistros, int nRemovidos){
	Ip *aux_rrn2;
	Isd *aux_rrn;
	Ir *copia;
	Ip* aux = (Ip *) malloc(sizeof(Ip));
	int flag[nregistros], primeira = 1, i = 0, h = 0, k = 0, ultima = 0, ultimaith = 0;

	for(int i = 0; i < nregistros; i++)
		flag[i] = 0;

	copia = (Ir *) malloc (sizeof(Ir) * MAX_REGISTROS);
	
	for (int j = 0; j < ntraj; j++){
		i = 0;
		copia[j] = iroute[j];
		
		while(copia[j].lista != NULL){
			strcpy(aux[i].pk, copia[j].lista->pk);
			copia[j].lista = copia[j].lista->prox;
			i++;
		}

		if(j == (ntraj-1))
			ultima = 1;

		for(int k = 0; k < nregistros; k++) {
			for(int h = 0; h < i; h++){
				if(strcmp(aux[h].pk, idate[k].pk) == 0){
					aux_rrn2 = (Ip *) bsearch(idate[k].pk, indice_primario, nregistros, sizeof(Ip), comparapk2);
				
					if(aux_rrn2 != NULL){
						if(ultima == 1){
							ultimaith++;	
						}
						exibir_registro(aux_rrn2->rrn);	
						if (ultimaith != i && aux_rrn2->rrn != -1)
							printf("\n");
					}		
				}
			}
		}
	}
}

int alterar(Ip* indice_primario, int nregistros){
	char pk[TAM_PRIMARY_KEY], vagas[TAM_VAGAS];

	scanf("%s", pk);

	Ip *aux;
	aux = (Ip *) bsearch(pk, indice_primario, nregistros+1, sizeof(Ip) , comparapk);

	if(aux == NULL || aux->rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
    }
	else{
        do{
       	getchar();scanf("%[^\n]s", vagas);
        }while(checaNovaVaga(vagas) == 0);
        
        Carona a = recuperar_registro(aux->rrn);
        
        strcpy(a.vagas, vagas);

		Ip *j = aux;

		//ALTERA O ARQUIVO
			char *p = ARQUIVO + (256 * j->rrn) + strlen(a.nome) + strlen(a.genero) + strlen(a.nascimento) +
			strlen(a.celular) + strlen(a.veiculo) + strlen(a.placa) + strlen(a.placa) + strlen(a.trajeto) 
			+ strlen(a.data) + strlen(a.hora) + strlen(a.valor) + 2;
			*p = a.vagas[0];
	}
}

int checaNovaVaga(char *vagas){
	double a;
	if (strlen(vagas) == 1){
		a = atof(vagas);
		if(a >= 0 && a <= 9)
			return 1;
	}
	
	printf(CAMPO_INVALIDO);
	return 0;
}

int remover(Ip *indice_primario, int nregistros, int *nRemovidos){
    Ip *a;
	char pk[TAM_PRIMARY_KEY];

	scanf("%[^\n]s", pk);
	//printf("%s\n", pk);
	a = (Ip*) bsearch(pk, indice_primario, nregistros+1, sizeof(Ip), comparapk2);
   	if(!a || a->rrn == -1){
   		printf(REGISTRO_N_ENCONTRADO);
        return 0;
   	}
    else{
       char *x = ARQUIVO + (256*a->rrn);
        *x = '*';
        *(x+1) = '|';
        a->rrn = -1;
        (*nRemovidos)++;

        return 1;
    }
}

void liberarEspaco(int *nregistros, int *ntraj, Ip* indice_primario, Ir* iroute, Is* idriver, Isd* idate, Ist* itime){
	int i, flag = 0, flag2 = 0, posTrajetoRemovido[MAX_REGISTROS], y = 0, contTrajetoRemovido = 0, contremovidos = 0;
    char* p;
	Carona j;
	Ip* aux_rrn;
	//printf("ENTROU LIBERAR ESPACO\n");
    //roda todo o arquivo
    for (int i = 0; i < *nregistros; i++){
        p = ARQUIVO + (256 * i);
        
        if(p[0] == '*'){
            //para cada posicao seguinte, o atual recebe o proximo para "tapar" o buraco
            strcpy(ARQUIVO+(256*i), p+256);
            flag = 1;
            contremovidos++;
    	}
	}
	if (flag == 1){
	
		for(int i = 0; i < *nregistros; i++){
			if(indice_primario[i].rrn == -1){
				for(int j = 0; j < *ntraj; j++){
					removerLista(&iroute[j].lista, indice_primario[i].pk);
					if(iroute[j].lista == NULL){
						posTrajetoRemovido[contTrajetoRemovido] = j;
						contTrajetoRemovido++;
					}
				}				
				for(y = 0; y < contTrajetoRemovido; y++){
					for(int k = posTrajetoRemovido[y]; k < *ntraj; k++){
						strcpy(iroute[k].trajeto, iroute[k+1].trajeto);
						iroute[k].lista = iroute[k+1].lista;
					}
					posTrajetoRemovido[y+1]--;
				}
			}
		}
		(*nregistros) -= contremovidos;
		//printf("VAI ENTRAR RECRIA INDICES\n");
		recriaIndices(indice_primario, idriver, idate, itime, nregistros, iroute, ntraj);
		//printf("SAIU RECRIA INDICES\n");
	}
	else
		return;
	
}

void recriaIndices(Ip* indice_primario, Is* idriver, Isd* idate, Ist* itime, int * nregistros, Ir* iroute, int *ntraj){
	 Carona j;
	free(idriver);
	free(idate);
	free(itime);
	//liberar_iroute(iroute, ntraj);
	(*ntraj) = 0;
	//free(iroute);
	free(indice_primario);

	//printf("DEPOIS DOS FREE\n");

	indice_primario = (Ip *)calloc(MAX_REGISTROS, sizeof(Ip));
	if (!indice_primario){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

    idriver = (Is *)calloc(MAX_REGISTROS, sizeof(Is));
	if (!idriver){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

    idate = (Isd *)calloc(MAX_REGISTROS, sizeof(Isd));
	if (!idate){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

    itime = (Ist *)calloc(MAX_REGISTROS, sizeof(Ist));
	if (!itime){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

    iroute = (Ir *) calloc (MAX_TRAJETOS, sizeof(Ir));
	if(!iroute){
	 	perror(MEMORIA_INSUFICIENTE);
	 	exit(1);
	}

	//printf("DEPOIS DOS CALLOC\n");

	criar_iprimary(indice_primario, nregistros);
	criar_idriver(idriver, nregistros) ;
	criar_idate(idate, nregistros);
	criar_itime(itime, nregistros);
	//printf("AUUUUU\n");
	criar_iroute(iroute, nregistros, ntraj);
	//printf("NREGISTROS: %d\n", *nregistros);
	//printf("NTRAJ: %d\n", *ntraj);
}

void liberar_iroute(Ir* iroute, int *ntraj){
	ll* a, *x;

	for(int i=0; i< *ntraj; i++){
		a = iroute[i].lista;
		while(a != NULL){
			x = a;
			a = a->prox;
			free(x);
		}
		//free(x);
		//free(iroute[i].trajeto);
	}
}