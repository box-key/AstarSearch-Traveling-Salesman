#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

class AStarNode {
	public:
		int ID;
		int numEdgeFromStart;
		int gStar;
		int hStar;
		int fStar;
		AStarNode* next;
		AStarNode* parent;
	
	AStarNode(){
		ID = -1;
		next = parent = NULL;
		gStar = hStar = fStar = 0;
	}
		
	AStarNode(int id, int numEdge){
		ID = id;
		numEdgeFromStart = numEdge;
		next = parent = NULL;
		gStar = hStar = fStar = 0;
	} 
};

class AStarSearch {
	public:
		int numNodes;
		int startID;
		int whichFunction;
		int numEdges;
		int minEdge;
		int costSum;
		int medianCost;
		int numEdgeFromStart;
		int closeListSize;
		int* childAry;
		int** costMatrix;
		AStarNode* OpenList;
		AStarNode* CloseList;
	
	AStarSearch(ifstream& inFile){
		inFile >> numNodes;
		numEdges = costSum = 0;
		loadMatrix(inFile);
		OpenList = new AStarNode();
		CloseList = NULL;
		medianCost = numEdgeFromStart = closeListSize = 0;
		getUserInput();
		AStarNode* SNode = new AStarNode(startID, 0);
		SNode->hStar = 0;
		SNode->gStar = 0;
		SNode->fStar = computeFStar(SNode);
		openInsert(SNode);
	}

	void startSearch(ofstream& outFile1){
		AStarNode* currentNode; 
		AStarNode* childNode;
		AStarNode* oldNode;
		int matrixIndex;
		while(!checkPath(CloseList)){
			currentNode = openRemove();
			matrixIndex = currentNode->ID;
			copyChildList(matrixIndex);
			for(int childIndex=1;childIndex<=numNodes;childIndex++){
				if(childAry[childIndex]>0){
					numEdgeFromStart = closeListSize;
					childNode = new AStarNode(childIndex, numEdgeFromStart);
					childNode->parent = currentNode;
					childNode->hStar = computeHStar(childNode, whichFunction);
					childNode->gStar = computeGStar(childNode);
					childNode->fStar = computeFStar(childNode);
					if(!isInCloseList(childNode, oldNode)){
						openInsert(childNode);	
					}
					else {
						if((childNode->fStar < oldNode->fStar) && childIndex != startID){
							// Remove old node and set better node on the path
							removeClose(oldNode);			
							closePush(childNode);
						}
					}
				}
			}
			closePush(currentNode);
		}
		tracePath(outFile1, CloseList);
	}
	
	void tracePath(ofstream& outFile, AStarNode* node){
		AStarNode* temp = node;
		int total = 0;
		outFile << startID << " " << temp->ID << " " << costMatrix[startID][temp->ID] << endl;
		total += costMatrix[startID][temp->ID];
		while(temp->next!=NULL){
			outFile << temp->ID << " " << temp->next->ID << " " << 	costMatrix[temp->ID][temp->next->ID] << endl;
			total += costMatrix[temp->ID][temp->next->ID];
			temp = temp->next;			
		}
		outFile << "\nThe total cost of the simple-path is: " << total;
	}
	
	bool checkPath(AStarNode* node){
		if(node == NULL) return false;
		AStarNode* temp = node;
		// If there is no path from the node to startNode, it returns true
		if(costMatrix[temp->ID][startID]<=0) return false;
		// Count the number of nodes on a path
		int numNodeOnPath = 1;
		while(temp->next != NULL){
			numNodeOnPath++;
			temp = temp->next;
		}
		// If the list contains all nodes, then returns true; otherwise, false
		if(numNodeOnPath == numNodes) return true;
		else return false;
	}
	
	bool isInCloseList(AStarNode* currentNode, AStarNode*& oldNode){
		AStarNode* temp = CloseList;
		if(temp == NULL) return false;
		while(temp->next != NULL) {
			if(temp->ID == currentNode->ID){
				oldNode = temp;
				return true;
			}
			temp = temp->next;
		}
 		if(temp->ID == currentNode->ID){
			oldNode = temp;
			return true;
		}
		return false;
	}
	
	int computeFStar(AStarNode* node){
		return node->gStar + node->hStar;
	}
	
	int computeGStar(AStarNode* node){
		return node->parent->gStar + costMatrix[node->ID][node->parent->ID];
	}
		
	int computeHStar(AStarNode* node, int whichFunction){
		int result = -1;
		switch(whichFunction){
			case 1:
				result = costSum / numEdges * (numNodes - node->numEdgeFromStart);
				break;
			case 2:
				result = minEdge * (numNodes - node->numEdgeFromStart);
				break;
			case 3:
				result = getMedian() * (numNodes - node->numEdgeFromStart);
				break;
		}
		return result;
	}
	
	int getMedian(){
		if(medianCost==0){
			// Get a temporary 1D array to store all edge cost
			int arraySize = (numNodes*numNodes) - numNodes - (((numNodes*numNodes) - numNodes)/2);
			int* temp = new int [arraySize];
			int k = 0;
			// Assign costs to temp
			for(int i=1;i<=numNodes-1;i++)
				for(int j=i+1;j<=numNodes;j++)
					temp[k++] = costMatrix[i][j];
			// Sort a half array to get median using selection sort
			int minIndex;
			for(int i=0;i<arraySize/2 + 1;i++){
				minIndex = i;
				for(int j=i+1;j<arraySize;j++)
					if(temp[j] < temp[minIndex])
						minIndex = j;
				swap(temp[i], temp[minIndex]);
			}
			// Assign the median value to medianCost
			if(arraySize%2 == 1) medianCost = temp[arraySize/2];
			else medianCost = (temp[(arraySize/2)-1] + temp[arraySize/2])/2;
		}
		return medianCost;
	}
	
	void swap(int& x, int& y){
		int temp = x;
		x = y;
		y = temp;
	}
	
	void getUserInput(){
		cout << "Enter the start node's ID: ";
		cin >> startID;
		while(startID <= 0 || numNodes < startID){
			cout << "Illegal input. There're " << numNodes << " nodes in this graph.\nEnter again: ";
			cin >> startID;
		}
		cout << "Which H function do you wanna use?: ";
		cin >> whichFunction;
		while(whichFunction<=0 || whichFunction>3){
			cout << "Illegal input. Enter 1, 2, or 3: ";
			cin >> whichFunction;
		}
	}
	
	void copyChildList(int matrixIndex){
		childAry = new int[numNodes+1];
		for(int i=0;i<=numNodes;i++){
			childAry[i] = costMatrix[i][matrixIndex];
		}
	}
	
	void removeClose(AStarNode* target){
		if(CloseList == NULL) return;
		AStarNode* temp = CloseList;
		if(temp->ID == target->ID){
			closeListSize--;
			CloseList = temp->next;
			return;
		}
		while((temp->next!=NULL) && (temp->next->ID != target->ID))
			temp = temp->next;
		if(temp->next == NULL) return;
		if(temp->next->ID == target->ID){
			temp->next = temp->next->next;
			closeListSize--;
		}
	}
	
	void closePush(AStarNode* newNode){
		closeListSize++;
		if(CloseList == NULL){
			CloseList = newNode;
			return;
		}
		newNode->next = CloseList;
		CloseList = newNode;
	}
	
	AStarNode* openRemove(){
		if(OpenList->next == NULL) return OpenList;
		AStarNode* temp = OpenList->next;
		if(temp->next == NULL){
			OpenList->next = NULL;
			return temp;
		}
		OpenList->next = temp->next;
		return temp;
	}
	
	void openInsert(AStarNode* newNode){
		if(OpenList->next == NULL){
			OpenList->next = newNode;
			return;
		}
		AStarNode* temp = OpenList;
		while((temp->next!=NULL) && (newNode->fStar > temp->next->fStar)){
			temp = temp->next;
		}
		newNode->next = temp->next;
		temp->next = newNode;
	}
	
	void printClose(ofstream& outFile, AStarNode* top){
		AStarNode* temp = top;
		outFile << "\n\n****** CLOSE Stack ******\n";
		if(temp == NULL){
			outFile << "NULL\n\n";
			return;
		}
		while(temp->next!=NULL){
			outFile << "(" << temp->ID << ", " << temp->fStar << ")-->";
			temp = temp->next;
		}
		outFile << "(" << temp->ID << ", " << temp->fStar << ")-->NULL\n\n";
	}
	
	void printOpen(ofstream& outFile, AStarNode* head){
		AStarNode* temp = head;
		outFile << "\n\n****** OPEN List ******\n";
		while(temp->next!=NULL){
			outFile << "(" << temp->ID << ", " << temp->hStar << ", " << temp->gStar << ", " << temp->fStar << ")-->";
			temp = temp->next;
		}
		outFile << "(" << temp->ID << ", " << temp->hStar << ", " << temp->gStar << ", " << temp->fStar << ")-->NULL\n\n";
	}
	
	void loadMatrix(ifstream& inFile){
		costMatrix = new int*[numNodes+1];
		for(int i=0;i<=numNodes;i++){
			costMatrix[i] = new int[numNodes+1];
			for(int j=0;j<=numNodes;j++)
				if(i == j) costMatrix[i][j] = 0;
				else costMatrix[i][j] = -1;
 		}
 		int node1, node2, cost;
 		bool isFirstCost = true;
 		while(inFile >> node1 >> node2 >> cost){
 			// Get minimun cost
			if(isFirstCost && cost>0) {
 				minEdge = cost;
 				isFirstCost = false;
			}
 			if(cost < minEdge) minEdge = cost;
			// Get the number of edges
			if(cost > 0) numEdges++;
			// Get the sum of cost
			costSum += cost;
			// Assign the cost to matrix
			costMatrix[node1][node2] = cost;
 			costMatrix[node2][node1] = cost;
 		}
 	}
		
};

int main(int argc, char** argv) {
	ifstream inFile;
	inFile.open(argv[1]);	
	ofstream outFile;
	outFile.open(argv[2]);
	AStarSearch* Asearch = new AStarSearch(inFile);
	Asearch->startSearch(outFile);
	inFile.close();
	outFile.close();
	return 0;
}
