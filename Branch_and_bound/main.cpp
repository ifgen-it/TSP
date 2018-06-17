#include <iostream>
#include <stdlib.h>
#include <ctime>
#pragma comment(linker, "/STACK:100000000000000")	//for increase stack memory
using namespace std;

//***************************************************
//*						                            *
//*                   INPUT DATA                    *
//*						                            *
//***************************************************

//WARNING:
//you can get Stackoverflow error or lack of dynamic memory when you run program with Cities about 40-60.
//it is need to make some optimizations to fix it.

//#define RAND_WEIGHT				//comment this string if you want to set your own weights between cities
const int Cities = 8;			//set the count of cities

#ifdef RAND_WEIGHT
	int W[Cities][Cities];		//prepair matrix for random weights
#endif // RAND_WEIGHT

const int E = 888;				//endless (infinity)

// here you can set your own weights between cities, but at first comment string: #define RAND_WEIGHT
#ifndef RAND_WEIGHT				

	//9
	/*int W[Cities][Cities] = {
	E, 30, 25, 17, 39, 18, 20, 14, 10,
	31, E, 26, 25, 30, 42, 10, 41, 11,
	28, 27, E, 29, 18, 40, 11, 10, 31,
	37, 16, 21, E, 17, 25, 21, 15, 54,
	19, 23, 25, 31, E, 19, 78, 63, 77,
	21, 27, 32, 16, 19, E, 45, 44, 23,
	11, 24, 52, 26, 59, 21, E, 11, 12,
	21, 37, 71, 29, 18, 40, 41, E, 11,
	40, 16, 21, 22, 17, 25, 21, 15, E
	};*/

	//8
	int W[Cities][Cities] = {
	E, 30, 25, 17, 39, 18, 20, 14,
	31, E, 26, 25, 30, 42, 10, 41,
	28, 27, E, 29, 18, 40, 11, 10,
	37, 16, 21, E, 17, 25, 21, 15,
	19, 23, 25, 31, E, 19, 78, 63,
	21, 27, 32, 16, 19, E, 45, 44,
	11, 24, 52, 26, 59, 21, E, 11,
	21, 37, 71, 29, 18, 40, 41, E
	};

	//7
	/*int W[Cities][Cities] = {
	E, 30, 25, 17, 39, 18, 20,
	31, E, 26, 25, 30, 42, 10,
	28, 27, E, 29, 18, 40, 11,
	37, 16, 21, E, 17, 25, 21,
	19, 23, 25, 31, E, 19, 78,
	21, 27, 32, 16, 19, E, 45,
	11, 24, 52, 26, 59, 21, E
	};*/

	//6 example from Book from Pak - Branch and border method
	/*int W[Cities][Cities] = {
	E, 68, 73, 24, 70, 9,
	58, E, 16, 44, 11, 92,
	63, 9, E, 86, 13, 18,
	17, 34, 76, E, 52, 70,
	60, 18, 3, 45, E, 58,
	16, 82, 11, 60, 48, E
	};*/

	//6
	/*int W[Cities][Cities] = {
	E, 30, 25, 17, 39, 18,
	31, E, 26, 25, 30, 42,
	28, 27, E, 29, 18, 40,
	37, 16, 21, E, 17, 25,
	19, 23, 25, 31, E, 19,
	21, 27, 32, 16, 19, E
	};*/

	//5
	/*int W[Cities][Cities] = {
	E, 30, 25, 17, 39,
	31, E, 26, 25, 30,
	28, 27, E, 29, 18,
	37, 16, 21, E, 17,
	19, 23, 25, 31, E
	};*/

	//4
	/*int W[Cities][Cities] = {
	E, 50, 25, 47,
	31, E, 26, 25,
	18, 27, E, 29,
	37, 16, 71, E
	};*/
	
#endif // !RAND_WEIGHT

//***************************************************
//*						                            *
//*               END OF INPUT DATA                 *
//*						                            *
//***************************************************

int Path[Cities][2];		//path with arcs, Path[i][0] - from , Path[i][1] - to
int PathCounter = 0;
int ResPath[Cities + 1];	//path like chain
int Step = 1;				//number of algorithm iterations

struct Node
{
	int W[Cities][Cities];		//weight matrix
	int NeedRow[Cities];		//1 - needed row, 0 - needless row
	int NeedCol[Cities];		//1 - needed col, 0 - needless col
	int Arc[2];					//Arc[0] - out node, Arc[1] - in node
	int ArcExist;				//1 - Arc exists, 0 - Arc not exists
	int LowBound;				//low bound of this set
	Node* Son1;					//pointer on son1
	Node* Son2;					//pointer on son2
	Node* Parent;				//pointer on parent
	int Leaf;					//1- leaf, 0 - not leaf, not considered for searching MinLowBound
};

Node* pTreeRoot = nullptr;
Node* pMinLeaf = nullptr;

void PrintArr(int W[][Cities]);
int FindMinInRow(int Arr[][Cities], int Row);
int FindMinInCol(int Arr[][Cities], int Col);
void SetDiagonal(int Arr[][Cities]);
void FindOneZero(int Arr[][Cities], int& Row, int& Col);
int AdductionWeightMatrix(Node* pRoot);
Node* MinLowBound(Node* pRoot);
void MakeTree(Node* pRoot);
bool GamiltonCheck(Node* pTemp);
void CheckForCyclesMatrix(Node* pTemp);
bool CheckForCyclesElement(int i, int j);

int main()
{
	system("mode con cols=165 lines=200000");

	//random generation of weight array
#ifdef RAND_WEIGHT
	srand(time(0));
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			W[i][j] = 10 + rand() % 60;
		}
	}
	for (int i = 0; i < Cities; i++)
	{
		W[i][i] = E;
	}
#endif // RAND_WEIGHT

	cout << "Travelling salesman problem. Branch and bound method" << endl << endl;
	PrintArr(W);
	cout << endl << "Number of cities is " << Cities << endl << endl;
	
	// Create first node - root
	Node* pRoot = new Node;
	if (pTreeRoot == nullptr)
	{
		pTreeRoot = pRoot;
	}
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			pRoot->W[i][j] = W[i][j];
		}
		pRoot->NeedRow[i] = 1;
		pRoot->NeedCol[i] = 1;

	}
	pRoot->Arc[0] = 0;
	pRoot->Arc[1] = 0;
	pRoot->ArcExist = 0;
	pRoot->LowBound = 0;
	pRoot->Son1 = nullptr;
	pRoot->Son2 = nullptr;
	pRoot->Parent = nullptr;
	pRoot->Leaf = 0;


	// Intro: Operation of adduction
	pRoot->LowBound += AdductionWeightMatrix(pRoot);

	// it is adducted matrix
	cout << endl << "W0 adducted weight matrix:" << endl;
	PrintArr(pRoot->W);
	cout << endl << "Lower bound for W0 = " << pRoot->LowBound << endl;
	
	// Main action: Many times iterations
	MakeTree(pRoot);

	// Finish
	cout << endl << "Run time: " << clock() / 1000.0 << " sec" << endl;
	cout << endl << "Steps: " << Step << endl;

	cout << endl << "Travelling salesman problem. Branch and bound method" << endl << endl;
	cout << "Source weight matrix:" << endl << endl;
	PrintArr(W);

	cout << endl << "end" << endl;
	system("pause");
	return 0;
}

//For each step of algorithm except last one we need to check:
//is there any arc in WeightMatrix, which can make a cycle (this cycle will be not Gamilton (not through all nodes))
void CheckForCyclesMatrix(Node* pTemp)
{
	//int Path[Cities][2];	global
	PathCounter = 0;	// global
	
	Node* pSource = pTemp;
	// Getting arcs from all nodes which prior min leaf node
	while (pTemp->Parent)
	{
		if (pTemp->ArcExist == 1)
		{
			Path[PathCounter][0] = pTemp->Arc[0];
			Path[PathCounter][1] = pTemp->Arc[1];
			PathCounter++;
		}
		pTemp = pTemp->Parent;
	}

	//cout << endl << "========= CheckForCyclesMatrix ============" << endl;

	//cout << endl << "The shortest path is " << pSource->LowBound << endl;
	//cout << endl << "Route goes through cities:" << endl << endl;
	for (int i = 0; i < PathCounter; i++)
	{
		//cout << Path[i][0] + 1 << " > " << Path[i][1] + 1 << endl;
	}

	// for all need rows and cols must check if that element can make a cycle
	//cout << endl << "print test matrix" << endl;
	for (int i = 0; i < Cities; i++)
	{
		bool WasPrint = false;
		for (int j = 0; j < Cities; j++)
		{
			//cout << pTemp->W[i][j] << " ";
			if (pSource->NeedRow[i] == 1 && pSource->NeedCol[j] == 1)
			{
				// checking for cycles
				if (pSource->W[i][j] < E/2)
				{
					bool DelElement = false;

					DelElement = CheckForCyclesElement(i, j);

					if (DelElement)
					{
						pSource->W[i][j] = E;
					}
					
				}
				//printf("%4d", pSource->W[i][j]);
				WasPrint = true;
				
			}
			
		}
		if (WasPrint)
		{
			//cout << endl;
		}
	}
	//cout << endl << "end of test" << endl;

}

bool CheckForCyclesElement(int i, int j)
{
	// i - node from arc out
	// j - node where arc in

	// we have path array
	// pathcounter is amount of arcs that we have on this moment
	//int Path[Cities][2];	global
	//PathCounter 	// global
	int TempRoute[Cities];
	int RouteCounter = 0;
	TempRoute[RouteCounter] = j;

	
	while (true)
	{
		bool HaveArc = false;
		for (int k = 0; k < PathCounter; k++)
		{
			
			if (Path[k][0] == TempRoute[RouteCounter])
			{
				HaveArc = true;
				TempRoute[RouteCounter + 1] = Path[k][1];
				RouteCounter++;
				if (TempRoute[RouteCounter] == i)
				{
					return true;	// find the cycle
				}
				break;
			}
		}
		if (!HaveArc)
		{
			return false;	// no cycle by i,j element
		}
	}

}

bool GamiltonCheck(Node* pTemp)
{
	int Path[Cities][2];
	int PathCounter = 0;

	int ResPath[Cities + 1];
	Node* pMinLeaf = pTemp;
	// Searching for shortest path
	//Node* pTemp = pMinLeaf;

	// Getting the Arc from min leaf node
	for (int i = 0; i < Cities; i++)
	{
		if (pTemp->NeedRow[i] == 1)
		{
			Path[PathCounter][0] = i;
		}
	}
	for (int i = 0; i < Cities; i++)
	{
		if (pTemp->NeedCol[i] == 1)
		{
			Path[PathCounter][1] = i;
		}
	}
	PathCounter++;


	// Getting arcs from all nodes which prior min leaf node
	while (pTemp->Parent)
	{
		if (pTemp->ArcExist == 1)
		{
			Path[PathCounter][0] = pTemp->Arc[0];
			Path[PathCounter][1] = pTemp->Arc[1];
			PathCounter++;
		}
		pTemp = pTemp->Parent;
	}

	//cout << endl << "==========================" << endl;

	//cout << endl << "The shortest path is " << pMinLeaf->LowBound << endl;
	//cout << endl << "Route goes through cities:" << endl << endl;
	for (int i = 0; i < Cities; i++)
	{
		//cout << Path[i][0] + 1 << " > " << Path[i][1] + 1 << endl;
	}

	//make route like chain
	ResPath[0] = Cities - 1;
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			if (ResPath[i] == Path[j][0])
			{
				ResPath[i + 1] = Path[j][1];
				break;
			}
		}
	}

	cout << endl << "Number of cities is " << Cities << endl;
	cout << endl << "The shortest path is " << pMinLeaf->LowBound << endl;
	cout << endl << "Route like chain:" << endl << endl;
	for (int i = 0; i < Cities; i++)
	{
		cout << ResPath[i] + 1 << " > ";
	}
	cout << ResPath[Cities] + 1;

	//checking for more than 1 cicle
	bool Gamilton = true;
	for (int i = 1; i < Cities; i++)
	{
		if (ResPath[i] == Cities - 1)
		{
			Gamilton = false;
			break;
		}
	}
	if (Gamilton)
	{
		cout << endl << endl << "Gamilton is good" << endl;
	}
	else
	{
		cout << endl << endl << "Gamilton is bad" << endl;
	}
	return Gamilton;
}

void MakeTree(Node* pRoot)
{
	// begin func
	cout << endl << "========= " << Step++ << " Step ==========" << endl << endl;
	// make 2 branches from Node
	// searching for arc wich will devide Node (set of possible routes) on 2 not-intersected sets of possible routes

	int Row = 0;
	int Col = 0;
	FindOneZero(pRoot->W, Row, Col);
	//cout << "Row= " << Row << " Col= " << Col << endl;

	//create 2 sons

	//first Son will not exist searched arc
	Node* pSon1 = new Node;
	pRoot->Son1 = pSon1;
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			pSon1->W[i][j] = pRoot->W[i][j];
		}
		pSon1->NeedRow[i] = pRoot->NeedRow[i];
		pSon1->NeedCol[i] = pRoot->NeedCol[i];
	}
	pSon1->Arc[0] = Row;
	pSon1->Arc[1] = Col;
	pSon1->ArcExist = 0;
	pSon1->LowBound = pRoot->LowBound;
	pSon1->Son1 = nullptr;
	pSon1->Son2 = nullptr;
	pSon1->Parent = pRoot;
	pSon1->Leaf = 1;

	//modifying weight matrix
	pSon1->W[Row][Col] = E;
	//cout << endl << "W1 weight matrix:" << endl;
	//PrintArr(pSon1->W);

	//adduction Sons1 weight matrix - another way, faster than AdductionWeightMatrix(pSon1);
	/*
	int NewMinInCol = FindMinInCol(pSon1->W, Col);
	for (int i = 0; i < Cities; i++)
	{
	pSon1->W[i][Col] -= NewMinInCol;
	}
	SetDiagonal(pSon1->W);

	int NewMinInRow = FindMinInRow(pSon1->W, Row);
	for (int i = 0; i < Cities; i++)
	{
	pSon1->W[Row][i] -= NewMinInRow;
	}
	SetDiagonal(pSon1->W);
	//correction
	pSon1->W[Row][Col] = E;
	*/

	pSon1->LowBound += AdductionWeightMatrix(pSon1);
	//correction
	pSon1->W[Row][Col] = E;
	//cout << endl << "W1 adducted weight matrix:" << endl;
	//PrintArr(pSon1->W);

	//lower border
	/*
	pSon1->LowBound += NewMinInCol + NewMinInRow;
	*/
	//cout << endl << "Lower bound for W1 = " << pSon1->LowBound << endl;
	//cout << "W1 arc: " << pSon1->Arc[0] + 1 << pSon1->Arc[1] + 1 << " is " << pSon1->ArcExist << endl;

	//end of Son1 creation

	//Second Son will exist searched arc
	Node* pSon2 = new Node;
	pRoot->Son2 = pSon2;
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			pSon2->W[i][j] = pRoot->W[i][j];
		}
		pSon2->NeedRow[i] = pRoot->NeedRow[i];
		pSon2->NeedCol[i] = pRoot->NeedCol[i];
	}
	pSon2->Arc[0] = Row;
	pSon2->Arc[1] = Col;
	pSon2->ArcExist = 1;
	pSon2->LowBound = pRoot->LowBound;
	pSon2->Son1 = nullptr;
	pSon2->Son2 = nullptr;
	pSon2->Parent = pRoot;
	pSon2->Leaf = 1;

	//modifying weight matrix
	pSon2->W[Col][Row] = E;
	pSon2->NeedRow[Row] = 0;
	pSon2->NeedCol[Col] = 0;
	for (int i = 0; i < Cities; i++)
	{
		pSon2->W[Row][i] = E;
		pSon2->W[i][Col] = E;
	}
	//cout << endl << "W2 weight matrix:" << endl;
	//PrintArr(pSon2->W);

	//adduction Sons1 weight matrix
	pSon2->LowBound += AdductionWeightMatrix(pSon2);

	//cout << endl << "W2 adducted weight matrix:" << endl;
	//PrintArr(pSon2->W);

	//lower border
	//cout << endl << "Lower bound for W2 = " << pSon2->LowBound << endl;
	//cout << "W2 arc: " << pSon2->Arc[0] + 1 << pSon2->Arc[1] + 1 << " is " << pSon2->ArcExist << endl;

	//end of Son2 creation


	// Searching for Leaf with min LowBound
	
	Node* pTempMin = nullptr;

	while (true)
	{
		pTempMin = MinLowBound(pTreeRoot);
		//cout << endl << "Min low bound is " << pTempMin->LowBound << endl;

		int CheckExistance = 0;
		for (int i = 0; i < Cities; i++)
		{
			CheckExistance += pTempMin->NeedCol[i];
		}

		if (CheckExistance <= 1)
		{
			if (GamiltonCheck(pTempMin) == false)
			{
				pTempMin->Leaf = 0;
				
			}
			else
			{
				pMinLeaf = pTempMin;
				return;
			}
		}
		else
		{
			CheckForCyclesMatrix(pTempMin);
			// recursive call for pTempMin Node
			MakeTree(pTempMin);
			return;
		}

	}
		


	// end func
}

//Searching for Node with Min Lowbound realized with recursion. You can get Stackoverflow because of it.
//So one way for optimization: Save LowBound of each Node in another global structure, for example, min priority queue STL. 
//If you need to get Min LowBound you need use DeleteMin function for this priority queue.

Node* MinLowBound(Node* pRoot)
{
	if (pRoot->Son1 == nullptr)
	{
		if (pRoot->Leaf == 1)
		{
			return pRoot;
		}
		else
		{
			return nullptr;
		}
	}
	Node* MinTree1 = MinLowBound(pRoot->Son1);
	Node* MinTree2 = MinLowBound(pRoot->Son2);
	if (MinTree1 && MinTree2)
	{
		if (MinTree1->LowBound < MinTree2->LowBound)
		{
			return MinTree1;
		}
		else
		{
			return MinTree2;
		}
	}
	else if (MinTree1 && !MinTree2)
	{
		return MinTree1;
	}
	else if (!MinTree1 && MinTree2)
	{
		return MinTree2;
	}
	else
	{
		return nullptr;
	}
}

int AdductionWeightMatrix(Node* pRoot)		//int Arr[][Cities])
{
	//calculate min for each row
	int WRowsMin[Cities] = { 0 };
	for (int i = 0; i < Cities; i++)
	{
		if (pRoot->NeedRow[i] == 1)
		{
			WRowsMin[i] = FindMinInRow(pRoot->W, i);
		}
	}
	//reduce each value of Weight matrix by corresponding WRowsMin value

	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			if (pRoot->NeedRow[i] == 1)
			{
				pRoot->W[i][j] -= WRowsMin[i];
			}
		}
	}
	SetDiagonal(pRoot->W);
	//cout << "After reduce min from rows:" << endl;
	//PrintArr(Arr);

	//calculate min for each col
	int WColsMin[Cities] = { 0 };
	for (int i = 0; i < Cities; i++)
	{
		if (pRoot->NeedCol[i] == 1)
		{
			WColsMin[i] = FindMinInCol(pRoot->W, i);
		}
	}
	//reduce each value of Weight matrix by corresponding WColsMin value

	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			if (pRoot->NeedCol[j] == 1)
			{
				pRoot->W[i][j] -= WColsMin[j];
			}
		}
	}
	SetDiagonal(pRoot->W);

	// lower bound of adducted W
	int F = 0;
	for (int i = 0; i < Cities; i++)
	{
		F = F + WRowsMin[i] + WColsMin[i];
	}
	return F;
}

//it is optimized way for searching one ZeroWeight in weight matrix, optimized for whole algotithm
//but it need more operation of comparision
void FindOneZero(int Arr[][Cities], int& Row, int& Col)
{
	int NumZero = 0;
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			if (Arr[i][j] == 0)
			{
				NumZero++;
			}
		}
	}
	//cout << "Num of Zero = " << NumZero << endl;

	int(*Temp)[3];
	Temp = new int[NumZero][3];
	int TempCounter = 0;

	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			if (Arr[i][j] == 0)
			{
				Temp[TempCounter][0] = i;
				Temp[TempCounter][1] = j;
				
				// copying Weight matrix
				int TempArr[Cities][Cities];
				for (int k = 0; k < Cities; k++)
				{
					for (int l = 0; l < Cities; l++)
					{
						TempArr[k][l] = Arr[k][l];
					}
				}
				// get addiction constance for Zero
				TempArr[i][j] = E;
				int NewMinInCol = FindMinInCol(TempArr, j);
				for (int z = 0; z < Cities; z++)
				{
					TempArr[z][j] -= NewMinInCol;
				}
				
				int NewMinInRow = FindMinInRow(TempArr, i);
				for (int z = 0; z < Cities; z++)
				{
					TempArr[i][z] -= NewMinInRow;
				}
				// update value
				Temp[TempCounter][2] = NewMinInCol + NewMinInRow;
				TempCounter++;
			}
		}
	}
	// searching for Max value of Sum of addiction constants
	int MaxInd = 0;
	int Max = Temp[0][2];

	for (int i = 1; i < NumZero; i++)
	{
		if (Temp[i][2] > Max)
		{
			MaxInd = i;
			Max = Temp[i][2];
		}
	}

	Row = Temp[MaxInd][0];
	Col = Temp[MaxInd][1];


	delete[] Temp;
}

//it is less optimized way for searching one ZeroWeight in weight matrix
//but it more faster
/*void FindOneZero(int Arr[][Cities], int& Row, int& Col)
{
	// searching for arc wich will devide Node
	Row = 0;
	Col = 0;
	int ZeroNum = 0;

	// first will search one zero in columns
	for (int i = 0; i < Cities; i++)
	{
		ZeroNum = 0;
		for (int j = 0; j < Cities; j++)
		{
			if (Arr[j][i] == 0)
			{
				ZeroNum++;
				Row = j;
			}
		}
		if (ZeroNum == 1)
		{
			Col = i;
			return;
		}
	}

	// then will search one zero in rows 
	for (int i = 0; i < Cities; i++)
	{
		ZeroNum = 0;
		for (int j = 0; j < Cities; j++)
		{
			if (Arr[i][j] == 0)
			{
				ZeroNum++;
				Col = j;
			}
		}
		if (ZeroNum == 1)
		{
			Row = i;
			return;
		}
	}
}*/

// uses to correct WeightMatrix after reducing
void SetDiagonal(int Arr[][Cities])
{
	for (int i = 0; i < Cities; i++)
	{
		Arr[i][i] = E;
	}
}

int FindMinInCol(int Arr[][Cities], int Col)
{
	int MinInd = 0;
	int Min = Arr[MinInd][Col];
	for (int i = 1; i < Cities; i++)
	{
		if (Arr[i][Col] < Min)
		{
			MinInd = i;
			Min = Arr[i][Col];
		}
	}
	return Min;
}

int FindMinInRow(int Arr[][Cities], int Row)
{
	int MinInd = 0;
	int Min = Arr[Row][MinInd];
	for (int i = 1; i < Cities; i++)
	{
		if (Arr[Row][i] < Min)
		{
			MinInd = i;
			Min = Arr[Row][i];
		}
	}
	return Min;
}

void PrintArr(int W[][Cities])
{
	for (int i = 0; i < Cities; i++)
	{
		for (int j = 0; j < Cities; j++)
		{
			printf("%4d",W[i][j]);
		}
		cout << endl;
	}
}