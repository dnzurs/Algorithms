//============================================================================//
//
//	File Name	: project_1.cpp
//	Create Date	: 17.03.2019
//	Designers	: Nebi Deniz Uras
//	Number 	    : 504181527
//	Description	: Analysis Of Algorithm 2 - Project 1
//
//	Important Notes:
//
//============================================================================//

//============================================================================//
//=============================== INCLUDES ===================================//
//============================================================================//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <cstring>

//============================================================================//
//==========================  MACRO DEFINITIONS ==============================//
//============================================================================//
#define MINE_SET_AREA	("s")
#define MINER_AREA		("m")
#define FREE_AREA		(".")

#define WRONG_AREA		("x")

#define GRAPH_NODE_MEMORY_COUNT		(10)

#define DEBUG (1)



//============================================================================//
//=========================== TYPE DEFINITIONS ===============================//
//============================================================================//
struct Node
{
	char *grid;
	Node *nodeAdjacents;

	void createGrid(const Node *parentNode);
	void addNodeToAdjacents(const Node *node);
};

struct CommonRules
{
	int row;
	int col;
	int gridSize;
	int *rowRuleGrid;
	int *colRuleGrid;
	char *rootGrid;

	// memory functions
	void getMemoryForGrids();
	void freeGridMemory();

	// initialize functions according to input file
	void initialize(const char *fileName);
	void createRootGrid(char ruleCount, int index);
	void createRowRuleGrid(int ruleCount, int index);
	void createColRuleGrid(int ruleCount, int index);

	// miner rule check functions according to instructions
	bool IsRowAvailable(Node *node);
	bool IsColAvailable(Node *node);
	bool IsMinerPositionCross(Node *node);
	bool IsMinerHasMinerNear(Node *node);
	bool IsMinerHasMineSiteNear(Node *node);
};

//============================================================================//
//========================== FUNCTION PROTOTYPES =============================//
//============================================================================//

//============================================================================//
//============================ GLOBAL VARIABLES ==============================//
//============================================================================//

CommonRules ruleTable;

Node *graphNodeList = NULL;
int	graphNodeListCount = 0;

//============================================================================//
//============================ PUBLIC FUNCTIONS ==============================//
//============================================================================//
int main(int argc, char ** argv)
{
	if (argc != 4)
	{
		std::cout << "ERROR : Invalid Argument\n" << std::endl;
	}
	else
	{
		char *searchMethod = argv[1];
		char *inputFile = argv[2];
		char *outputFile = argv[3];		

		graphNodeList = (Node *)malloc(sizeof(Node *) * GRAPH_NODE_MEMORY_COUNT);

		ruleTable.initialize(inputFile);



		//free memory
		ruleTable.freeGridMemory();
		//nodelarýda sil
		free(graphNodeList);
	}

#if _WIN32
	system("Pause");
#endif	
	return 0;
}

void updateGraphNodeList(const Node *newNode)
{
	if (graphNodeListCount < GRAPH_NODE_MEMORY_COUNT)
	{
		*(graphNodeList + graphNodeListCount) = *newNode;
	}
	else
	{
		graphNodeList = (Node *)realloc(graphNodeList, sizeof(Node *) * (graphNodeListCount + GRAPH_NODE_MEMORY_COUNT));
	}
}

#if DEBUG
void printRowColGrid(const int *grid, char *message)
{
	printf("\n");
	printf("%s\n", message);
	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		if ((i) % ruleTable.col == 0)
		{
			printf("\n");
		}
		printf("%d ", *(grid + i));
	}
	printf("\n\n");
}

void printMapGrid(const char *grid, char *message)
{
	printf("\n");
	printf("%s\n", message);
	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		if ((i) % ruleTable.col == 0)
		{
			printf("\n");		}

		printf("%c ", *(grid + i));
	}
	printf("\n\n");
}
#endif

//============================================================================//
//============================ PRIVATE FUNCTIONS =============================//
//============================================================================//

/**************************** FUNCTIONS OF COMMONRULES ************************/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::initialize(const char *fileName)
{
	FILE *fd = NULL;

	fd = fopen(fileName, "r");
	if (fd)
	{
		//int tempArray[MEMBER_COUNT_OF_TREE] = { 0 };
		int tempData = 0;
		int lastData = 0;

		fscanf(fd, "%d", &col);
		fscanf(fd, "%d", &row);
		gridSize = col * row;

		getMemoryForGrids();

		// fill column rule grid
		for (int i = 1; i <= col; i++)
		{
			int colRuleCount = 0;

			fscanf(fd, "%d", &colRuleCount);

			createColRuleGrid(colRuleCount, i);
		}
		 
		// fill row rule grid and root mine grid
		for (int i = 0; i < row; i++)
		{
			int rowRuleCount = 0;
			char rowCharachter = ' ';

			// fill row rule grid
			fscanf(fd, "%d", &rowRuleCount);
			for (int k = i * col; k < (i + 1) * col; k++)
			{
				createRowRuleGrid(rowRuleCount, k);
			}

			// fill root grid
			for (int j = i * col; j < (i + 1) * col; j++)
			{
				fscanf(fd, "\t%c", &rowCharachter);

				createRootGrid(rowCharachter, j);
			}
		}
	}

#if DEBUG
	printf("Column : %d - Row : %d\n", ruleTable.col, ruleTable.row);
	printRowColGrid(colRuleGrid, "Column Rule Grid");
	printRowColGrid(rowRuleGrid, "Row Rule Grid");
	printMapGrid(rootGrid, "Mine Site Default Grid");
#endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::getMemoryForGrids()
{
	rootGrid = (char *)malloc(sizeof(char) * gridSize);
	rowRuleGrid = (int *)malloc(sizeof(int) * gridSize);
	colRuleGrid = (int *)malloc(sizeof(int) * gridSize);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::createRootGrid(char ruleCount, int index)
{
	*(rootGrid + index) = ruleCount;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::createRowRuleGrid(int ruleCount, int index)
{
	*(rowRuleGrid + index) = ruleCount;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::createColRuleGrid(int ruleCount, int index)
{
	for (int i = index - 1; i < gridSize; i += col)
	{
		*(colRuleGrid + i) = ruleCount;
	}
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::freeGridMemory()
{
	free(rootGrid);
	free(rowRuleGrid);
	free(colRuleGrid);
};