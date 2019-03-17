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
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stdio.h>
#include <string.h>

//============================================================================//
//==========================  MACRO DEFINITIONS ==============================//
//============================================================================//
#define MINE_SITE_AREA			('s')
#define MINER_AREA				('m')
#define FREE_AREA				('.')

#define GRAPH_NODE_COUNT	(10)

#define DEBUG	(0)

//============================================================================//
//=========================== TYPE DEFINITIONS ===============================//
//============================================================================//
struct Node
{
	Node **nodeAdjacents;
	int nodeAdjacentsCount;
	char *grid;
	int maxChildNode;
	bool isSuitableNode;
	int existingMinerCount;
	int *minerAreaIndexList;
	bool isNodeProcessed;
	bool isTraversed;

	void addNodeToAdjacents(Node *node);
};

struct Graph
{
	Node **graphNodeList;
	int	graphNodeCount;
	int graphNodeCountMax;
	int visitNode;
	int maxNumNodeKeptMem;
	bool isFound;
	Node *searchResultNode;

	void BFS();
	void DFS(Node *root);
	void addNewNode(Node *parentNode, int index);
	void updateGraphNodeList(Node *newNode);
	void createGraph();
	Node *IsNodeExistInGraph(Node *node);
	void writeToFile(char *fileName);
};

struct CommonRules
{
	int row;
	int col;
	int gridSize;
	int availableArea;
	int mineSiteCount;
	int *rowRuleGrid;
	int *colRuleGrid;
	char *rootGrid;
	int *minerAreaIndexList;

	// memory functions
	void getMemoryForGrids();
	void freeGridMemory();

	// initialize functions according to input file
	void initialize(const char *fileName);
	void createRootGrid(char character, int index);
	void createRowRuleGrid(int ruleCount, int index);
	void createColRuleGrid(int ruleCount, int index);

	// miner rule check functions according to instructions
	bool IsCheckNodeSuitable(Node *node, int minerIndex);
	bool IsRowAvailable(Node *node, int minerIndex);
	bool IsColAvailable(Node *node, int minerIndex);
	bool IsMinerHasMinerNear(Node *node, int minerIndex);
	bool IsMinerHasMineSiteNear(Node *node, int minerIndex);
};

//============================================================================//
//========================== FUNCTION PROTOTYPES =============================//
//============================================================================//
void upper_string(char *);

//============================================================================//
//============================ GLOBAL VARIABLES ==============================//
//============================================================================//
CommonRules ruleTable;
Graph graph;

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

		upper_string(searchMethod);

		graph.graphNodeList = (Node **)malloc(sizeof(Node *) * GRAPH_NODE_COUNT);

		ruleTable.initialize(inputFile);

		graph.createGraph();

		time_t start = clock();

		if (strcmp("DFS", searchMethod) == 0)
		{
			graph.maxNumNodeKeptMem = 0;
			graph.visitNode = 0;
			graph.DFS(graph.graphNodeList[0]);
		}
		else if (strcmp("BFS", searchMethod) == 0)
		{
			graph.BFS();
		}

		time_t runningTime = clock() - start;

		graph.writeToFile(outputFile);

		if (graph.isFound == true)
		{
			// print results
			std::cout << "Algorithm: " << searchMethod << std::endl;
			std::cout << "Number of the visited nodes : " << graph.visitNode << std::endl;
			std::cout << "Maximum number of nodes kept in the memory: " << graph.maxNumNodeKeptMem << std::endl;
			std::cout << "Running time : " << (double)runningTime / 1000 << " seconds." << std::endl;
			std::cout << "Solution is written to the file." << std::endl;

			graph.writeToFile(outputFile);
		}
		else
		{
			std::cout << "It is not found, please enter correct inputs\n";
		}
		
		//free memory
		ruleTable.freeGridMemory();
		for (int i = 0; i < graph.graphNodeCount; i++)
		{
			free(graph.graphNodeList[i]->nodeAdjacents);
			free(graph.graphNodeList[i]);
		}
		free(graph.graphNodeList);
	}

#if _WIN32
	system("Pause");
#endif	
	return 0;
}

void upper_string(char *s) 
{
	int c = 0;

	while (s[c] != '\0') 
	{
		if (s[c] >= 'a' && s[c] <= 'z') 
		{
			s[c] = s[c] - 32;
		}
		c++;
	}
}

#if DEBUG
void printRowColGrid(const int *grid, std::string message)
{
	std::cout << message;
	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		if ((i) % ruleTable.col == 0)
		{
			std::cout << "\n";
		}
		std::cout << *(grid + i) << " ";
	}
	std::cout << "\n" << std::endl;
}

void printMapGrid(const char *grid, std::string message)
{
	std::cout << message;
	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		if ((i) % ruleTable.col == 0)
		{
			std::cout << "\n";
		}
		std::cout << *(grid + i) << " ";
	}
	std::cout << "\n" << std::endl;
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
		int tempInt = -1;
		char lastChar;

		fscanf(fd, "%d", &col);
		fscanf(fd, "%d", &row);
		gridSize = col * row;

		getMemoryForGrids();

		// check if there is a character in first row rule line 
		fscanf(fd, "%d", &tempInt);
		if (tempInt == -1)
		{
			fscanf(fd, "\t%c", &lastChar);
		}

		// fill column rule grid
		for (int i = 1; i <= col; i++)
		{
			int colRuleCount = 0;

			if (tempInt != -1)
			{
				colRuleCount = tempInt;
				createColRuleGrid(colRuleCount, i);
				tempInt = -1;
			}
			else
			{
				fscanf(fd, "%d", &colRuleCount);
				createColRuleGrid(colRuleCount, i);
			}
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
	std::cout << "Column : " << ruleTable.col << " - Row : " << ruleTable.row << " - Available Area : " << ruleTable.availableArea << " - Mine Site : " << ruleTable.mineSiteCount << std::endl;
	printRowColGrid(colRuleGrid, std::string("Column Rule Grid"));
	printRowColGrid(rowRuleGrid, std::string("Row Rule Grid"));
	printRowColGrid(minerAreaIndexList, std::string("Miner Area Index Grid"));
	printMapGrid(rootGrid, std::string("Mine Site Default Grid"));
#endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::getMemoryForGrids()
{
	rootGrid			= (char *)malloc(sizeof(char) * gridSize);
	rowRuleGrid			= (int *)malloc(sizeof(int) * gridSize);
	colRuleGrid			= (int *)malloc(sizeof(int) * gridSize);
	minerAreaIndexList	= (int *)malloc(sizeof(int) * gridSize);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CommonRules::createRootGrid(char character, int index)
{
	*(rootGrid + index) = character;

	if (character == FREE_AREA)
	{
		availableArea++;
		*(minerAreaIndexList + index) = 0;
	}
	else if (character == MINE_SITE_AREA)
	{
		mineSiteCount++;
		*(minerAreaIndexList + index) = -1;
	}
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
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CommonRules::IsCheckNodeSuitable(Node *node, int minerIndex)
{
	bool flag = true;

	flag = IsRowAvailable(node, minerIndex);
	if (flag == true)
	{
		flag = IsColAvailable(node, minerIndex);
		if (flag == true)
		{
			flag = IsMinerHasMineSiteNear(node, minerIndex);
			if (flag == true)
			{
				flag = !IsMinerHasMinerNear(node, minerIndex); // if IsMiner return is true, it is not suitable
			}
		}
	}

	return flag;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CommonRules::IsRowAvailable(Node *node, int minerIndex)
{
	bool flag = true;
	int rowRule = ruleTable.rowRuleGrid[minerIndex];

	if (rowRule == 0)
	{
		flag = false;
	}
	else
	{
		int rowStartIndex = minerIndex - (minerIndex % ruleTable.col);
		int minerCountInRow = 0;

		for (int i = rowStartIndex; i < rowStartIndex + col; i++)
		{
			if (node->grid[i] == 'm')
			{
				minerCountInRow++;
			}
		}

		if (minerCountInRow > rowRule)
		{
			flag = false;
		}
	}

	return flag;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CommonRules::IsColAvailable(Node *node, int minerIndex)
{
	bool flag = true;
	int colRule = ruleTable.colRuleGrid[minerIndex];

	if (colRule == 0)
	{
		flag = false;
	}
	else
	{
		int colStartIndex = minerIndex % ruleTable.col;
		int minerCountInCol = 0;

		for (int i = colStartIndex; i <= (colStartIndex + (row - 1) * col) ; i += col)
		{
			if (node->grid[i] == 'm')
			{
				minerCountInCol++;
			}
		}

		if (minerCountInCol > colRule)
		{
			flag = false;
		}
	}

	return flag;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CommonRules::IsMinerHasMinerNear(Node *node, int minerIndex)
{
	bool flagUp			= false;
	bool flagUpRight	= false;
	bool flagUpLeft		= false;
	bool flagDown		= false;
	bool flagDownRight	= false;
	bool flagDownLeft	= false;
	bool flagRight		= false;
	bool flagLeft		= false;

	// check up side of miner whether has a miner or not
	if ((minerIndex - ruleTable.col >= 0) && (node->grid[minerIndex - ruleTable.col] == MINER_AREA))
	{
		flagUp = true;
		return true;
	}

	// check up-right side of miner whether has a miner or not
	if ((minerIndex - ruleTable.col >= 0) && (minerIndex % ruleTable.col != ruleTable.col - 1) && (node->grid[minerIndex - ruleTable.col + 1] == MINER_AREA))
	{
		flagUpRight = true;
		return true;
	}

	// check up-left side of miner whether has a miner or not
	if ((minerIndex - ruleTable.col >= 0) && (minerIndex % ruleTable.col != 0) && (node->grid[minerIndex - ruleTable.col - 1] == MINER_AREA))
	{
		flagUpLeft = true;
		return true;
	}

	// check down side of miner whether has a miner or not
	if ((minerIndex + ruleTable.col < ruleTable.gridSize) && (node->grid[minerIndex + ruleTable.col] == MINER_AREA))
	{
		flagDown = true;
		return true;
	}

	// check down-right side of miner whether has a miner or not
	if ((minerIndex + ruleTable.col < ruleTable.gridSize) && (minerIndex % ruleTable.col != ruleTable.col - 1) && (node->grid[minerIndex + ruleTable.col + 1] == MINER_AREA))
	{
		flagDownRight = true;
		return true;
	}

	// check down-left side of miner whether has a miner or not
	if ((minerIndex + ruleTable.col < ruleTable.gridSize) && (minerIndex % ruleTable.col != 0) && (node->grid[minerIndex + ruleTable.col - 1] == MINER_AREA))
	{
		flagDownLeft = true;
		return true;
	}

	// check right side of miner whether has a miner or not
	if ((minerIndex % ruleTable.col != ruleTable.col - 1) && (node->grid[minerIndex + 1] == MINER_AREA))
	{
		flagRight = true;
		return true;
	}

	// check left side of miner whether has a miner or not
	if ((minerIndex % ruleTable.col != 0) && (node->grid[minerIndex - 1] == MINER_AREA))
	{
		flagLeft = true;
		return true;
	}

	return (flagUp || flagUpRight || flagUpLeft || flagDown || flagDownRight || flagDownLeft || flagRight || flagLeft);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CommonRules::IsMinerHasMineSiteNear(Node *node, int minerIndex)
{
	bool flagUp		= true;
	bool flagDown	= true;
	bool flagRight	= true;
	bool flagLeft	= true;

	// check up side of miner has a mine site
	if ((minerIndex - ruleTable.col >= 0) && (node->grid[minerIndex - ruleTable.col] != MINE_SITE_AREA))
	{
		flagUp = false;
	}
	else if (minerIndex - ruleTable.col < 0)
	{
		flagUp = false;
	}

	// check down side of miner has a mine site
	if ((minerIndex + ruleTable.col < ruleTable.gridSize) && (node->grid[minerIndex + ruleTable.col] != MINE_SITE_AREA))
	{
		flagDown = false;
	}
	else if (minerIndex + ruleTable.col > ruleTable.gridSize - 1)
	{
		flagDown = false;
	}

	// check right side of miner has a mine site
	if ((minerIndex % ruleTable.col != ruleTable.col - 1) && (node->grid[minerIndex + 1] != MINE_SITE_AREA))
	{
		flagRight = false;
	}
	else if (minerIndex % ruleTable.col == ruleTable.col - 1)
	{
		flagRight = false;
	}

	// check left side of miner has a mine site
	if ((minerIndex % ruleTable.col != 0) && (node->grid[minerIndex - 1] != MINE_SITE_AREA))
	{
		flagLeft = false;
	}
	else if (minerIndex % ruleTable.col == 0)
	{
		flagLeft = false;
	}

	return (flagUp || flagDown || flagRight || flagLeft);
};

/******************************* FUNCTIONS OF GRAPH ***************************/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::updateGraphNodeList(Node *newNode)
{
	int increaseCount = 0;

	if (graphNodeCount + 1 > graphNodeCountMax)
	{
		if (graphNodeCountMax > 100000)
		{
			graphNodeList = (Node **)realloc(graphNodeList, sizeof(Node *) * (graphNodeCount * 3 / 2));
			graphNodeCountMax = graphNodeCount * 3 / 2;
		}
		else
		{
			graphNodeList = (Node **)realloc(graphNodeList, sizeof(Node *) * (graphNodeCount * 2));
			graphNodeCountMax = graphNodeCount * 2;
		}
	}

	*(graphNodeList + graphNodeCount) = newNode;
	graphNodeCount++;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::createGraph()
{
	Node *root = (Node *)malloc(sizeof(Node));

	isFound = false;
	searchResultNode = NULL;
	visitNode = 0;
	graphNodeCount = 0;
	graphNodeCountMax = GRAPH_NODE_COUNT;

	root->nodeAdjacents = NULL;
	root->nodeAdjacentsCount = 0;
	root->minerAreaIndexList = (int *)malloc(sizeof(int) * ruleTable.gridSize);
	root->existingMinerCount = 0;
	root->isSuitableNode = true;
	root->maxChildNode = ruleTable.availableArea;
	root->grid = (char *)malloc(sizeof(char) * ruleTable.gridSize);

	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		*(root->grid + i) = *(ruleTable.rootGrid + i);
	}

	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		*(root->minerAreaIndexList + i) = *(ruleTable.minerAreaIndexList + i);
	}

	root->isTraversed = false;
	root->isNodeProcessed = true;
	graph.updateGraphNodeList(root);

	// first level child nodes for root
	for (int k = 0; k < ruleTable.gridSize; k++)
	{
		if (root->minerAreaIndexList[k] == 0)
		{
			addNewNode(root, k);
		}
	}

	// find nodes for childs
	for (int i = 0; i < graphNodeCount; i++)
	{
		if (graphNodeList[i]->existingMinerCount == ruleTable.mineSiteCount)
		{
			graphNodeList[i]->isNodeProcessed = true;
		}

		if ((graphNodeList[i]->isNodeProcessed == false) && (graphNodeList[i]->isSuitableNode == true))
		{
			for (int j = 0; j < ruleTable.gridSize; j++)
			{
				if (graphNodeList[i]->minerAreaIndexList[j] == 0)
				{
					addNewNode(graphNodeList[i], j);
				}
			}
		}
		graphNodeList[i]->isNodeProcessed = true;
	}

#if DEBUG
	if (graphNodeList[graphNodeCount-1]->isSuitableNode == true)
	{
		printf("Node Grid of %d", graphNodeCount - 1);
		printMapGrid(graph.graphNodeList[graphNodeCount - 1]->grid, "");
	}
#endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::addNewNode(Node *parentNode, int index)
{
	Node *newNode = (Node *)malloc(sizeof(Node));

	newNode->nodeAdjacents = NULL;
	newNode->nodeAdjacentsCount = 0;
	newNode->minerAreaIndexList = (int *)malloc(sizeof(int) * ruleTable.gridSize);
	newNode->existingMinerCount = parentNode->existingMinerCount + 1;
	newNode->isSuitableNode = false;
	newNode->maxChildNode = parentNode->maxChildNode - 1;
	newNode->grid = (char *)malloc(sizeof(char) * ruleTable.gridSize);

	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		*(newNode->grid + i) = *(parentNode->grid + i);
	}

	for (int i = 0; i < ruleTable.gridSize; i++)
	{
		*(newNode->minerAreaIndexList + i) = *(parentNode->minerAreaIndexList + i);
	}

	*(newNode->minerAreaIndexList + index) = 1;
	*(newNode->grid + index) = MINER_AREA;
	newNode->isNodeProcessed = false;
	newNode->isSuitableNode = false;

	// check whether node is suitable or not according to instructions
	if (ruleTable.IsCheckNodeSuitable(newNode, index))
	{
		newNode->isSuitableNode = true;
	}

	if (newNode->isSuitableNode == true)
	{
		Node *existNode = graph.IsNodeExistInGraph(newNode);
		if (existNode == NULL)
		{
			parentNode->addNodeToAdjacents(newNode);
			graph.updateGraphNodeList(newNode);
		}
		else
		{
			parentNode->addNodeToAdjacents(existNode);
			free(newNode);
		}
	}
	else
	{
		free(newNode);
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Node * Graph::IsNodeExistInGraph(Node *node)
{
	Node *existNode = NULL;
	bool isExist = false;

	for (int i = 0; i < graph.graphNodeCount; i++)
	{
		existNode = graphNodeList[i];
		for (int k = 0; k < ruleTable.gridSize; k++)
		{
			if (existNode->grid[k] != node->grid[k])
			{
				existNode = NULL;
				break;
			}
		}

		if (existNode != NULL)
		{
			break;
		}
	}

	return existNode;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::writeToFile(char *fileName)
{
	std::ofstream outFile;

	outFile.open(fileName);

	outFile << ruleTable.col << "\t" << ruleTable.row << "\n";

	outFile << " \t";
	for (int i = 0; i < ruleTable.col; i++)
	{
		outFile << ruleTable.colRuleGrid[i] << "\t";
	}
	outFile << "\n";

	for (int i = 0; i < ruleTable.row; i++)
	{
		outFile << ruleTable.rowRuleGrid[i * ruleTable.col] << "\t";
		for (int k = i*ruleTable.col; k < (i + 1)*ruleTable.col; k++)
		{
			outFile << graphNodeList[graphNodeCount - 1]->grid[k] << "\t";
		}
		outFile << "\n";
	}

	outFile.close();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::BFS()
{
	Node **nodeQueue = (Node**)malloc(sizeof(Node *));
	maxNumNodeKeptMem = 0;
	visitNode = 0;

	for (int i = 0; i < graphNodeCount; i++)
	{
		visitNode++;

		if (graphNodeList[i]->isSuitableNode == true)
		{
			nodeQueue = (Node**)realloc(nodeQueue, sizeof(Node *) * (maxNumNodeKeptMem + 1));
			*(nodeQueue + maxNumNodeKeptMem) = graphNodeList[i];
			maxNumNodeKeptMem++;
		}

		if (graphNodeList[i]->existingMinerCount == ruleTable.mineSiteCount)
		{
			isFound = true;
			searchResultNode = graphNodeList[i];
			break;
		}
	}
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Graph::DFS(Node *root)
{
	Node * currNode = root;
	Node * nextNode = NULL;
	
	if (isFound == true)
	{
		searchResultNode = currNode;
		return;
	}

	visitNode++;

	if (currNode->existingMinerCount == ruleTable.mineSiteCount)
	{
		isFound = true;
		searchResultNode = currNode;
	}
	else
	{
		if (currNode->nodeAdjacentsCount > 0)
		{
			for (int i = 0; i < currNode->nodeAdjacentsCount; i++)
			{
				DFS(currNode->nodeAdjacents[i]);

			}
		}

		maxNumNodeKeptMem++;
	}
}

/******************************* FUNCTIONS OF NODE ****************************/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Node::addNodeToAdjacents(Node *node)
{
	if (nodeAdjacentsCount == 0)
	{
		nodeAdjacents = (Node **)malloc(sizeof(Node *));
	}
	else
	{
		nodeAdjacents = (Node **)realloc(nodeAdjacents, sizeof(Node *) * (nodeAdjacentsCount + 1));
	}

	*(nodeAdjacents + nodeAdjacentsCount) = node;
	nodeAdjacentsCount++;
}