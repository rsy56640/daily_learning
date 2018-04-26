#include <stdio.h>  
#include <vector>   
#include <algorithm>
#include <set>
#include <unordered_map>
#include <stack>
using namespace std;

//POJ 1094 Sorting It All Out

struct node
{
	int ref;
	char character;
	vector<node*> next;
	node(char c) :character(c), next(0), ref(0) {}
	node() :character(0), next(0), ref(0) {}
};

int main()
{
	char buffer[5];
	int n, m;
	while (~scanf("%d%d", &n, &m))
	{
		if (n == 0)break;

		//read the m lines of relations
		set<char> nodes;
		unordered_map<char, node*> nodes_info;
		for (int i = 0; i < m; ++i)
		{
			char a, b;
			scanf("%s", buffer); sscanf(buffer, "%c<%c", &a, &b);
			nodes.insert(a);
			nodes.insert(b);
			try { nodes_info.at(b); }
			catch (std::out_of_range& e) { nodes_info[b] = new node(b); }
			try { nodes_info.at(a); }
			catch (std::out_of_range& e) { nodes_info[a] = new node(a); }
			vector<node*>::iterator it = nodes_info[a]->next.begin();
			for (; it != nodes_info[a]->next.end(); ++it)
				if (*it == nodes_info[b])break;
			if (it == nodes_info[a]->next.end())
			{
				nodes_info[a]->next.push_back(nodes_info[b]);
				++nodes_info[b]->ref;
			}
		}

		//no possibility to form a relation
		if (n > m + 1)
		{
			//check out whether there exists a circle
			stack<char> stk;
			int counter = 0;
			for (set<char>::iterator it = nodes.begin(); it != nodes.end(); ++it)
				if (nodes_info[*it]->ref == 0)
				{
					stk.push(*it);
					//nodes.erase(*it);
					counter++;
				}
			while (!stk.empty())
			{
				char c = stk.top(); stk.pop();
				for (vector<node*>::iterator it = nodes_info[c]->next.begin();
					it != nodes_info[c]->next.end(); ++it)
				{
					if (--(*it)->ref == 0)
					{
						stk.push((*it)->character);
						//nodes.erase(temp->next[i]->character);
						counter++;
					}
				}
			}
			if (nodes.size() > counter)
			{
				printf("Inconsistency found after %d relations.\n", m);
				continue;
			}
			//no circle, so not determined
			printf("Sorted sequence cannot be determined.\n");
			continue;
		}

		//has possibility to form the relation
		else
		{
			char* result = new char[n];
			int cur_pos = 0;
			stack<char> stk;
			//vector<char> toClear;
			int counter = 0;
			for (set<char>::iterator it = nodes.begin(); it != nodes.end(); ++it)
				if (nodes_info[*it]->ref == 0)
				{
					stk.push(*it);
					counter++;
					//toClear.push_back(*it);
				}
			//for (vector<char>::iterator it = toClear.begin(); it != toClear.end(); ++it)
				//nodes.erase(*it);
			bool isDetermined = true;
			while (!stk.empty())
			{
				if (stk.size() > 1)isDetermined = false;
				char c = stk.top(); stk.pop();
				result[cur_pos++] = c;
				for (vector<node*>::iterator it = nodes_info[c]->next.begin();
					it != nodes_info[c]->next.end(); ++it)
				{
					if (--((*it)->ref) == 0)
					{
						stk.push((*it)->character);
						//nodes.erase(temp->next[i]->character);
						counter++;
					}
				}
			}
			if (nodes.size() > counter)
				printf("Inconsistency found after %d relations.\n", m);
			else if (!isDetermined)
				printf("Sorted sequence cannot be determined.\n");
			else {
				printf("Sorted sequence determined after %d relations: ", n);
				for (int i = 0; i < n; ++i)
					printf("%c", result[i]);
				printf(".\n");
			}
			delete[] result;
		}
	}

	//system("pause");
	return 0;
}
