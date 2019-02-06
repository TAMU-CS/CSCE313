#include <iostream>
//Blank  A
//you need to include vector
#include <vector>

//Blank B
//this allows the default namespace to be std
using namespace std;

class node{
	//Blank C
	//by default member variables are private if not specified in a class
	public:
	int val;
	node* next;
};

void create_LL(vector<node *>& mylist, int node_num){
	mylist.assign(node_num, NULL);
	
	//create a set of nodes
	for(int i = 0; i < node_num; i++){
		//blank D
		mylist[i] = new node();
		//pointer objects need -> arrow to access member variables
		mylist[i]->val = i;
		mylist[i]->next = NULL;
	}
	
	//create linked list
	for(int i = 0; i < node_num - 1; i++){
		mylist[i]->next = mylist[i+1];
	}
}

int add_LL(node* ptr){
	int ret = 0;
	while(ptr){
		ret+= ptr->val;
		ptr = ptr->next;
	}
	return ret;
}

int main(int argc, char** argv){
	const int NODE_NUM = 3; 
	vector<node*> mylist;
	
	create_LL(mylist, NODE_NUM);
	int ret = add_LL(mylist[0]);
	cout << "The sum of nodes in LL is " << ret << endl;
	
	//Step4: delete nodes
	//Blank E
	for(int i = 0; i < mylist.size(); i++){
		delete mylist[i];
	}

	return 0;
}
