#include <iostream>
using namespace std;

class Tree
{
private:
	struct node
	{
		int key;
		node* left, * right;
	};
	node* root;
public:
	Tree()
	{
		root = nullptr;
	}
	node* newNode(int key)
	{
		node* Node = new node();
		Node->key = key;
		Node->left = Node->right = nullptr;
		return (Node);
	}

	node* rightRotate(node* x)
	{
		node* y = x->left;
		x->left = y->right;
		y->right = x;
		return y;
	}

	node* leftRotate(node* x)
	{
		node* y = x->right;
		x->right = y->left;
		y->left = x;
		return y;
	}

	node* splay(node* root, int key)
	{
		if (root == nullptr || root->key == key)
			return root;

		if (root->key > key)
		{
			if (root->left == nullptr) return root;

			if (root->left->key > key)
			{
				root->left->left = splay(root->left->left, key);
				root = rightRotate(root);
			}
			else if (root->left->key < key) // Zig-Zag (Left Right)
			{
				root->left->right = splay(root->left->right, key);
				if (root->left->right != nullptr)
					root->left = leftRotate(root->left);
			}
			return (root->left == nullptr) ? root : rightRotate(root);
		}
		else // Key lies in right subtree
		{
			if (root->right == nullptr) return root;

			// Zig-Zag (Right Left)
			if (root->right->key > key)
			{
				root->right->left = splay(root->right->left, key);
				if (root->right->left != nullptr)
					root->right = rightRotate(root->right);
			}
			else if (root->right->key < key)// Zag-Zag (Right Right)
			{
				root->right->right = splay(root->right->right, key);
				root = leftRotate(root);
			}

			return (root->right == nullptr) ? root : leftRotate(root);
		}
	}

	node* Insert(node* root, int k)
	{
		if (root == nullptr) return newNode(k);

		root = splay(root, k);

		if (root->key == k) return root;

		node* newnode = newNode(k);

		if (root->key > k)
		{
			newnode->right = root;
			newnode->left = root->left;
			root->left = nullptr;
		}

		else
		{
			newnode->left = root;
			newnode->right = root->right;
			root->right = nullptr;
		}

		return newnode; // newnode becomes new root
	}

	void Insert(int k)
	{
		root = Insert(root, k);
	}

	node* Delete(node* root, int key)
	{
		node* temp;
		if (!root)
			return NULL;
		root = splay(root, key);
		if (key != root->key)
			return root;
		else
		{
			if (!root->left)
			{
				temp = root;
				root = root->right;
			}
			else
			{
				temp = root;
				root = splay(root->left, key);
				root->right = temp->right;
			}
			delete temp;
			return root;
		}
	}

	void Delete(int k)
	{
		root = Delete(root, k);
	}

	int preOrder(node* root, const int& t, int& c)
	{
		int data = 0;
		if (root != nullptr)
		{
			data = preOrder(root->left, t, c);
			if (t == c && data != 0)
			{
				return data;
			}
			if (t != c)
			{
				c++;
			}
			else
			{
				return root->key;
			}
			data = preOrder(root->right, t, c);
			if (t == c && data != 0)
			{
				return data;
			}
		}
		return 0;
	}

	int Print_stat(int t)
	{
		int count = 0;
		return preOrder(root, t, count);
	}


};




int main()
{
	int n;//size
	cin >> n;
	Tree tree;
	int temp;
	for (int i = 0; i < n; i++)
	{
		cin >> temp;
		if (temp > 0)
		{
			tree.Insert(temp);
		}
		else
		{
			tree.Delete(-temp);
		}
		cin >> temp;
		cout << tree.Print_stat(temp) << endl;
	}
	return 0;
}