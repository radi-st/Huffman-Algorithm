#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iterator>

std::map<char, unsigned int> getFrequencyTable(std::string filename)
{
	std::map<char, unsigned int> frequencies;
	std::ifstream myfile(filename);

	if (myfile.is_open()) {
		char ch;
		while (myfile.get(ch))
		{
			if (frequencies.find(ch) != frequencies.end())
			{
				frequencies[ch] += 1;
			}
			else {
				frequencies[ch] = 1;
			}

		}
	}
	else throw "File not found!";
	myfile.close();
	return frequencies;
}

struct HuffNode
{
	char data;
	unsigned int frequency;
	HuffNode* left;
	HuffNode* right;

	HuffNode(const char& var_data, const unsigned int var_frequency, HuffNode* var_left = nullptr, HuffNode* var_right = nullptr) :
		data(var_data), frequency(var_frequency), left(var_left), right(var_right) {}

	bool isLeaf() const
	{
		return (this->left == nullptr && this->right == nullptr);
	}

};

/*
int frequencyCompare(HuffNode* rhs, HuffNode* lhs)
{
	if (rhs->frequency > lhs->frequency)
		return 1;
	if (rhs->frequency < lhs->frequency)
		return -1;

	return 0;
}
*/

class HuffTree
{
private:
	HuffNode* root;
public:
	HuffTree(HuffNode* var_root) : root(var_root) {}

	~HuffTree()
	{
		deleteNodes(root);
	}

	void deleteNodes(HuffNode* node)
	{
		if (node != nullptr)
		{
			deleteNodes(node->left);
			deleteNodes(node->right);
			delete node;
		}
	}

	HuffNode* getRoot() const
	{
		return root;
	}
	void tie(HuffNode* elem)
	{
		root = new HuffNode(NULL, root->frequency + elem->frequency, elem, root);
	}

};

// 'A':5,  'B':2,  'C':1,  'D':1, 'R': 2

//no divide & conquer, sorry :(

std::map<char, unsigned int>::iterator bsmapLower_Bound(unsigned int value, std::map<char, unsigned int>& mp)
{
	auto max_it = mp.begin();

	for (auto it = mp.begin(); it != mp.end(); ++it)
	{
		if (it->second > max_it->second)
		{
			max_it = it;
		}
	}


	auto lower_bound_it = max_it;
	for (auto it = mp.begin(); it != mp.end(); ++it)
	{
		if (it->second >= value && it->second < lower_bound_it->second)
		{
			lower_bound_it = it;
		}
	}

	return lower_bound_it;

}

HuffTree* getHuffmanTree(std::map<char, unsigned int> frequencies)
{
	char key_min_frequency{ frequencies.begin()->first };
	unsigned int min_frequency{ frequencies.begin()->second };

	for (const auto& p : frequencies)
	{
		if (p.second < min_frequency)
		{
			key_min_frequency = p.first;
			min_frequency = p.second;
		}
	}

	auto tree = new HuffTree(new HuffNode(key_min_frequency, min_frequency));
	frequencies.erase(key_min_frequency);

	while (!frequencies.empty())
	{
		auto it = bsmapLower_Bound(min_frequency, frequencies);
		tree->tie(new HuffNode(it->first, it->second));
		key_min_frequency = it->first;
		min_frequency = it->second;
		frequencies.erase(key_min_frequency);
	}

	return tree;

}

void encryptionPreOrder(HuffNode* root, std::string str, std::map<char, std::string>& encryptions)
{
	if (root != nullptr)
	{
		if (root->isLeaf())
		{
			encryptions[root->data] = str;
			return;
		}
		encryptionPreOrder(root->left, str + '0', encryptions);
		encryptionPreOrder(root->right, str + '1', encryptions);
	}

}
//main arguments , also debug mode
void encrypt(const HuffTree* tree, std::string input, std::string output)
{

	std::map<char, std::string> encryptions;

	encryptionPreOrder(tree->getRoot(), "", encryptions);

	std::ifstream i_file(input);
	std::ofstream o_file(output);
	if (i_file.is_open()) {
		char ch;
		while (i_file.get(ch))
		{
			o_file << encryptions[ch];
		}
	}
	else throw "Input file was not found";
	i_file.close();


}

void compress(std::string input, std::string output)
{
	//these variables are for readability of code

	auto frequencies = getFrequencyTable(input);
	auto hufftree = getHuffmanTree(frequencies);

	encrypt(hufftree, input, output);
	delete hufftree;

}





int main()
{
	compress("input.txt", "output.txt");
	return 0;
}
