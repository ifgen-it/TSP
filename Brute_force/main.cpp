#include <iostream>
#include <stdlib.h>
#include <ctime>

#include "myTemplates.h"	//uses for queue
#pragma comment(linker, "/STACK:100000000000000")
using namespace std;

//***************************************************
//*						                            *
//*                   INPUT DATA                    *
//*						                            *
//***************************************************

//WARNING:
//you can wait a lot of time to get result for Cities = 11.
//it is need to make some optimizations to increase results for few cities more

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


int Counter = 0;

int* Results = nullptr;
int ResCount = 0;

int* Path = nullptr;
int PathCounter = Cities - 1;

int FindRoute = 0;

//problem of searching for all routes is a problem of construction Decision tree
//number of routes equal number of all leafs is (Cities - 1)!
//but number of all Decision tree's nodes is more bigger, this number counted experementally in this programm

//Decision tree structure will realized using List of sons
//List of sons realized using pointers

struct SonNode;

//node of Decision tree
struct Node
{
	int City;
	int Weight;					// arc weight from parent to this node
	int SumWeight;				// sum weight from root node to this node
	SonNode* Sons;				// pointer to First son (List of sons)
	
	int RouteOwn[Cities];		// service route, which modifies during tree construction
								// 0 on RouteOwn[i] position means that it is need to make son with 'i' City.

	int RouteForChild[Cities];	// service route, which intend for inheritance for all sons
								// sons must know what cities have been constructed before (in the route to the root)
								// and what cities sons must constuct by itself
								// 1 - city was constructed (city is in route from root to node), 0 - city was not constructed yet.
	int Level;					// level of Tree's Node, root level is 1

};

struct SonNode
{
	Node* Son;
	SonNode* pNext;
};

MyQueue<Node*> Q1(1);

void PrintArr(int W[][Cities], int N);
void MakeTree();
void PrintTree(Node* pRoot);
int FindMin();
void SearchPath(Node* pRoot);
long int Fact(int x);
long int NumNodes(int N);


int main()
{
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

	system("mode con cols=80 lines=655");
		
	cout << "Travelling salesman problem. Brute force method" << endl << endl;
	PrintArr(W, Cities);

	// Amount of routes = (Cities - 1)!
	int NumRoutes = 1;
	for (int i = 2; i < Cities; i++)
	{
		NumRoutes = NumRoutes * i;
	}

	cout << endl << "Number of cities: " << Cities << endl;
	cout << endl << "Number of possible different routes: " << NumRoutes << endl;
	cout << endl << "Theoretical number of Decision tree nodes: " << NumNodes(Cities) << endl;
	
	Results = new int[NumRoutes];
	
	Node* pRoot = new Node;
	pRoot->City = Cities;
	pRoot->Weight = 0;
	pRoot->SumWeight = 0;
	pRoot->Sons = nullptr;
	pRoot->Level = 1;

	for (int i = 0; i < Cities; i++)
	{pRoot->RouteOwn[i] = 0;
	pRoot->RouteForChild[i] = 0;}
	pRoot->RouteOwn[Cities - 1] = 1;
	pRoot->RouteForChild[Cities - 1] = 1;


	Q1.Enqueue(pRoot);
	MakeTree();

	/*PrintTree(pRoot);
	cout << endl;
	for (int i = 0; i < ResCount; i++)
	{
		cout << i+1 << " " << Results[i] << endl;
	}*/

	int MinRoute = FindMin();
	FindRoute = MinRoute;
	
	cout << endl << "Run time: " << clock()/1000.0 << " sec" << endl;
	cout << endl << "The shortest route is " << MinRoute << endl;

	cout << endl << "This route goes through cities:" << endl << endl;

	// Searching for shortest path
	Path = new int[Cities + 1];
	Path[0] = Path[Cities] = Cities;
	
	SearchPath(pRoot);

	for (int i = 0; i < Cities; i++)
	{
		cout << Path[i] << " > ";
	}
	cout << Path[Cities];
	cout << endl << endl << "Experimental number of Decision tree nodes: " << Counter << endl;
	cout << endl << "end" << endl;

	system("pause");
	return 0;
}

int FindMin()
{
	int MinInd = 0;
	int Min = Results[MinInd];
	for (int i = 1; i < ResCount; i++)
	{
		if (Results[i] < Min)
		{
			MinInd = i;
			Min = Results[MinInd];
		}
	}
	return Min;
}

void PrintTree(Node* pRoot)
{
	cout << Counter++ << " " << pRoot->City << " " << pRoot->Weight << " " << pRoot->SumWeight << " ";
		for (int i = 0; i < Cities; i++)
		{
			cout << pRoot->RouteForChild[i];
		}
		cout << " " << endl;
		 
	SonNode* pTemp = pRoot->Sons;
	while (pTemp)
	{
		PrintTree(pTemp->Son);
		pTemp = pTemp->pNext;
	}
}

void SearchPath(Node* pRoot)
{
	
	SonNode* pTemp = pRoot->Sons;
	while (pTemp)
	{
		SearchPath(pTemp->Son);
		pTemp = pTemp->pNext;
	}

	if (pRoot->Level == PathCounter + 1)
	{
		if (FindRoute == pRoot->SumWeight + W[pRoot->City - 1][Path[PathCounter+1]-1])
		{
			Path[PathCounter--] = pRoot->City;
			FindRoute = pRoot->SumWeight;

		}
	}
}

void MakeTree()
{
	
		try
		{
			while (true)
			{
				Node* pRoot = Q1.Dequeue();
				
				//cout << Counter++ << " " << pRoot->City << " " << pRoot->Weight << " " << pRoot->SumWeight << " " << endl;
				Counter++;
				/*for (int i = 0; i < Cities; i++)
				{
				cout << pRoot->RouteOwn[i];
				}
				cout << " " << endl;*/

				for (int i = 0; i < Cities; i++)
				{
					if (pRoot->RouteOwn[i] == 0)
					{
						// create son of parent
						pRoot->RouteOwn[i] = 1;

						Node* tempSon = new Node;
						tempSon->City = i + 1;
						tempSon->Weight = W[pRoot->City - 1][tempSon->City - 1];
						tempSon->SumWeight = pRoot->SumWeight + tempSon->Weight;
						tempSon->Sons = nullptr;
						tempSon->Level = pRoot->Level + 1;

						for (int j = 0; j < Cities; j++)
						{
							tempSon->RouteOwn[j] = pRoot->RouteForChild[j];
							tempSon->RouteForChild[j] = pRoot->RouteForChild[j];
						}
						tempSon->RouteOwn[i] = 1;
						tempSon->RouteForChild[i] = 1;

						// create SonNode
						SonNode* pTemp = new SonNode;
						pTemp->pNext = nullptr;
						pTemp->Son = tempSon;

						//update info at parent's list of sons:
						if (pRoot->Sons == nullptr)
						{
							pRoot->Sons = pTemp;
						}
						else
						{
							SonNode* pTempSonNode = pRoot->Sons;
							while (pTempSonNode->pNext != nullptr)
							{
								pTempSonNode = pTempSonNode->pNext;
							}
							pTempSonNode->pNext = pTemp;	// add new son 
						}
						//add son to queue
						Q1.Enqueue(tempSon);

					}
				}
				// this node is last in route, next node is finish-start
				if (pRoot->Level == Cities)	// leaf
				{
					Results[ResCount++] = pRoot->SumWeight + W[pRoot->City - 1][Cities - 1];
				}
			}
		}
		catch (const char* err)
		{
			//cout << err << endl;
		}
		
	
}

void PrintArr(int W[][Cities], int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			//cout << W[i][j] << " ";
			printf("%4d",W[i][j]);
		}
		cout << endl;
	}
}

long int NumNodes(int N)
{
	long int res = Fact(N - 1);

	for (int k = 1; k <= N - 1; k++)
	{
		res = res + Fact(N - 1) / Fact(N - k);
	}
	return res;
}

long int Fact(int x)
{
	long res = 1;
	for (int i = 2; i <= x; i++)
	{
		res = res * i;
	}
	return res;
}