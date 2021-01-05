#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iterator>

std::map<char, unsigned int> getFrequencyTable(std::string filename)
{
	std::map<char, unsigned int> frequencies;
	std::ifstream i_file(filename);

	if (i_file.is_open()) {
		char ch;
		while (i_file.get(ch))
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
	i_file.close();
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
	o_file.close();


}

void compress(std::string input, std::string output)
{

	unsigned int bit_count{ 0 };
	auto frequencies = getFrequencyTable(input);

	auto hufftree = getHuffmanTree(frequencies);

	encrypt(hufftree, input, output);

	std::ofstream info_file("info_for_decompression.txt");
	for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
	{
		info_file << it->first << '\n' << it->second << '\n';
	}
	delete hufftree;

}



std::map<char, unsigned int> readFrequencyTable()
{
	std::map<char, unsigned int> mp;
	std::ifstream info_file("info_for_decompression.txt");

	if (info_file.is_open())
	{
		std::string key;
		std::string value;
		while (std::getline(info_file, key) && std::getline(info_file, value))
		{
			mp[key[0]] = std::stoi(value);

		}
	}
	else throw "The file containing info for decompression was not found!";
	info_file.close();
	return mp;

}

void decompress(std::string input, std::string output)
{
	auto frequencies = readFrequencyTable();
	auto hufftree = getHuffmanTree(frequencies);
	auto current_node = hufftree->getRoot();
	std::ifstream i_file(input);
	std::ofstream o_file(output);
	if (i_file.is_open()) {
		char ch;
		while (i_file.get(ch))
		{
			
			if (ch == '0') 
			{ 
				current_node = current_node->left; 
			}
			else
			{
				current_node = current_node->right;
			}
			if (current_node->isLeaf()) 
			{
				o_file << current_node->data;
				current_node = hufftree->getRoot();
			}
		}
	}
	else throw "Input file not found!";
	i_file.close();
	o_file.close();


}








int main()
{
	compress("input.txt", "output.txt");
	decompress("output.txt", "decompression.txt");
	return 0;
}
