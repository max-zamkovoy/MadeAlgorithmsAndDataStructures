#include <iostream>
#include <stack>

using namespace std;

class Tree
{
private:
	struct Node
	{
		int t_data;
		Node* left;
		Node* right;
		Node(const int& data)
		{
			t_data = data;
			left = nullptr;
			right = nullptr;
		}
	};
	Node* tree;
public:
	Tree()
	{
		tree = nullptr;
	}
	~Tree()
	{
		Clear();
	}

	void Add(int data)
	{
		if (!tree)
		{
			tree = new Node(data);
			return;
		}
		Node* root = tree;
		Node* elem = new Node(data);
		while (root != nullptr)
		{
			if (elem->t_data > root->t_data)
			{
				if (root->right != nullptr)
				{
					root = root->right;
				}
				else
				{
					root->right = elem;
					break;
				}
			}
			else if (elem->t_data <= root->t_data)
			{
				if (root->left != nullptr)
				{
					root = root->left;
				}
				else
				{
					root->left = elem;
					break;
				}
			}
		}
	}
	void Print()
	{
		Node* root = tree;
		std::stack<const Node*> stack;

		while (root != nullptr || !stack.empty())
		{
			while (root != nullptr)
			{
				stack.push(root);
				root = root->left;
			}

			std::cout << stack.top()->t_data << ' ';
			root = stack.top()->right;
			stack.pop();
		}
	}

	void Clear()
	{
		while (tree != nullptr)
		{
			while (tree != nullptr)
			{
				tree = tree->left;
			}

			Node* elem = tree;
			delete tree;
			if (elem != nullptr)
			{
				tree = elem->right;
			}
			else
			{
				tree = nullptr;
			}
		}
	}
};






int main()
{
	int n;//size
	cin >> n;
	Tree tree;
	for (int i = 0; i < n; i++)
	{
		int temp;
		cin >> temp;
		tree.Add(temp);
	}
	tree.Print();
	return 0;
}