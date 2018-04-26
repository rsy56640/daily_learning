#include <stdio.h>  
#include <vector>   
#include <algorithm>
using namespace std;

//POJ 3368 Frequent values

class SegmentTree {

	struct SegmentTreeNode
	{
		int start, end;
		int  _max;
		SegmentTreeNode(int start, int end, int max) {
			this->start = start;
			this->end = end;
			this->_max = max;
		}
	};

public:

	SegmentTree(vector<int>& Vec)
		:_size(1),
		_Identity_Element_for_MAX(0x80000000)
	{

		find_size(Vec.size());
		ST.resize(2 * _size - 1);
		build(0, 0, _size - 1, Vec);

	}


	int queryMAX(int start, int end) const
	{
		return doQueryMAX(0, start, end);
	}


protected:

	int _size;

	vector<SegmentTreeNode*> ST;

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
				(start < Vec.size()) ? Vec[start] : _Identity_Element_for_MAX);
			return;
		}

		//internal node (non-leaf)
		int mid = (start + end) / 2;


		//construct this node with initial val(_Identity_Element)
		ST[index] = new SegmentTreeNode(start, end, _Identity_Element_for_MAX);


		//construct left and right subTree (recursion)
		build((index << 1) + 1, start, mid, Vec);
		build((index << 1) + 2, mid + 1, end, Vec);


		//set value
		ST[index]->_max = max(ST[(index << 1) + 1]->_max,
			ST[(index << 1) + 2]->_max);

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
	int n, q;
	while (~scanf("%d%d", &n, &q))
	{
		if (n == 0)continue;
		vector<int> num(n);
		for (int i = 0; i < n; ++i)
			scanf("%d", &num[i]);
		vector<int> frequence(n);	//记录每个区间的长度
		vector<int> interval(n); interval[0] = 0;	//区间，记录每个不同区间的start
		int counter = 0, prev = 0;
		for (int i = 1; i < n; ++i)
		{
			if (num[i] != num[i - 1])
			{
				frequence[counter++] = i - prev;
				interval[counter] = i;
				prev = i;
			}
		}
		frequence[counter++] = n - prev;
		frequence.resize(counter);
		interval.resize(counter);
		SegmentTree ST(frequence);
		for (int i = 0; i < q; ++i)
		{
			int start, end;
			scanf("%d%d", &start, &end);
			if (start == end)
			{
				printf("%d\n", 1);
				continue;
			}
			start--; end--;
			if (counter == 1)
			{
				printf("%d\n", end - start + 1);
				continue;
			}
			if (start >= interval[counter - 1])
			{
				printf("%d\n", end - start + 1);
				continue;
			}
			//分开两边，中间用线段树查询max，再和两边取max
			//分离左边
			int low = 0, high = counter - 1;
			while (high - low > 1)
			{
				if (interval[(low + high) / 2] > start)
					high = (low + high) / 2;
				else low = (low + high) / 2;
			}
			int _start = high, left = interval[high] - start;
			//分离右边
			int _end, right;
			if (end >= interval[counter - 1])
			{
				_end = counter - 2, right = end - interval[counter - 1] + 1;
			}
			else {
				low = 0, high = counter - 1;
				while (high - low > 1)
				{
					if (interval[(low + high) / 2] > end)
						high = (low + high) / 2;
					else low = (low + high) / 2;
				}
				_end = low - 1, right = end - interval[low] + 1;
			}
			int frequent;
			if (_start > _end) frequent = 0;
			else frequent = ST.queryMAX(_start, _end);
			frequent = max(frequent, left);
			frequent = max(frequent, right);
			printf("%d\n", frequent);
		}
		getchar();
	}

	//system("pause");
	return 0;
}
