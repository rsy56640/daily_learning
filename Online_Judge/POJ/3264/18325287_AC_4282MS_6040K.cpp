#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 3264 Balanced Lineup

class SegmentTree {

	struct SegmentTreeNode
	{
		int start, end;
		int _min, _max;
		SegmentTreeNode(int start, int end, int min, int max) {
			this->start = start;
			this->end = end;
			this->_min = min;
			this->_max = max;
		}
	};

public:

	SegmentTree(vector<int>& Vec)
		:_size(1),
		_Identity_Element_for_MIN(0x7fffffff),
		_Identity_Element_for_MAX(0)
	{
		// do intialization if necessary  
		if (Vec.empty())return;

		find_size(Vec.size());
		ST.resize(2 * _size - 1);
		build(0, 0, _size - 1, Vec);

	}


	int queryMIN(int start, int end) const
	{
		return doQueryMIN(0, start, end);
	}

	int queryMAX(int start, int end) const
	{
		return doQueryMAX(0, start, end);
	}

protected:

	int _size;

	vector<SegmentTreeNode*> ST;

	const int _Identity_Element_for_MIN;

	const int _Identity_Element_for_MAX;

private:

	//find_size
	void find_size(int size)
	{
		while (_size < size)
		{
			_size <<= 1;
		}
	}


	//SegmentTree Initialization
	void build(int index, int start, int end, const vector<int>& Vec)
	{

		//leaf node
		if (start == end)
		{
			ST[index] = new SegmentTreeNode(start, end,
				(start < Vec.size()) ? Vec[start] : _Identity_Element_for_MIN,
				(start < Vec.size()) ? Vec[start] : _Identity_Element_for_MAX);
			return;
		}

		//internal node (non-leaf)
		int mid = (start + end) / 2;


		//construct this node with initial val(_Identity_Element)
		ST[index] = new SegmentTreeNode(start, end,
			_Identity_Element_for_MIN, _Identity_Element_for_MAX);


		//construct left and right subTree (recursion)
		build((index << 1) + 1, start, mid, Vec);
		build((index << 1) + 2, mid + 1, end, Vec);


		//set value
		ST[index]->_min = min(ST[(index << 1) + 1]->_min,
			ST[(index << 1) + 2]->_min);
		ST[index]->_max = max(ST[(index << 1) + 1]->_max,
			ST[(index << 1) + 2]->_max);

	}


	//index: cur_node
	int doQueryMIN(int index, int start, int end) const
	{

		//no segment union
		if (start > ST[index]->end || end < ST[index]->start)
			return _Identity_Element_for_MIN;


		//querying segment includes root segment
		if (start <= ST[index]->start && ST[index]->end <= end)
			return ST[index]->_min;


		//partially coincide
		return min(doQueryMIN((index << 1) + 1, start, end),
			doQueryMIN((index << 1) + 2, start, end));

	}


	//index: cur_node
	int doQueryMAX(int index, int start, int end) const
	{

		//no segment union
		if (start > ST[index]->end || end < ST[index]->start)
			return _Identity_Element_for_MAX;


		//querying segment includes root segment
		if (start <= ST[index]->start && ST[index]->end <= end)
			return ST[index]->_max;


		//partially coincide
		return max(doQueryMAX((index << 1) + 1, start, end),
			doQueryMAX((index << 1) + 2, start, end));

	}

};
int main() {
	int N, Q;
	scanf("%d%d", &N, &Q);
	vector<int> height(N);
	for (int i = 0; i < N; ++i)
		scanf("%d", &height[i]);
	SegmentTree ST(height);
	for (int i = 0; i < Q; ++i)
	{
		int start, end;
		scanf("%d%d", &start, &end);
		printf("%d\n", ST.queryMAX(start - 1, end - 1) - ST.queryMIN(start - 1, end - 1));
	}

	//system("pause");
	return 0;
}
