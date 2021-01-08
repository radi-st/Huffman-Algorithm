//Terminal examples
//$env:DEBUG=1; ./Huffman_Algorithm -c -i input.txt -o output.txt
//$env:DEBUG=0; ./Huffman_Algorithm -d -i output.txt -o input.txt

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iterator>
#include <exception>


//constructs the frequency table needed fo the algorithm
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
	else throw "Input file not found!";
	i_file.close();
	return frequencies;
}



//The node of the tree that huffman algorithm builds
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

//the tree itself
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

	//builds a tree consisting of the previous tree, the new element given and makes the new root the sum of their frequencies
	void tie(HuffNode* elem)
	{
		root = new HuffNode(NULL, root->frequency + elem->frequency, elem, root);
	}

};


//no divide & conquer, sorry :(
std::map<char, unsigned int>::iterator mapLower_Bound(unsigned int value, std::map<char, unsigned int>& mp)
{
	//finds the element with the biggest frequency
	auto max_it = mp.begin();

	for (auto it = mp.begin(); it != mp.end(); ++it)
	{
		if (it->second > max_it->second)
		{
			max_it = it;
		}
	}

	//the lower bound we are looking for is <= max element
	auto lower_bound_it = max_it;

	//so we begin looking for a smaller lower bound that is >=value
	for (auto it = mp.begin(); it != mp.end(); ++it)
	{
		if (it->second >= value && it->second < lower_bound_it->second)
		{
			lower_bound_it = it;
		}
	}

	return lower_bound_it;

}


//constructs the huffman algorith tree
HuffTree* getHuffmanTree(std::map<char, unsigned int> frequencies)
{
	//we find the element with the minimal frequency so we can use it as the initial element in our tree
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

	//we don't need it anymore so we remove it, map is called by value so no worries
	frequencies.erase(key_min_frequency);

	//now we look for lower bound and then we tie
	while (!frequencies.empty())
	{
		auto it = mapLower_Bound(min_frequency, frequencies);
		tree->tie(new HuffNode(it->first, it->second));
		key_min_frequency = it->first;
		min_frequency = it->second;
		frequencies.erase(key_min_frequency);
	}

	return tree;

}


// each char (leaf) from the tree is written as key in a map and its value is set as the path leading to it 
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

//final compression function
void compress(std::string input, std::string output)
{
	//these 2 variables are made for calculation of degree of compression
	unsigned int bit_count{ 0 };
	unsigned int byte_count{ 0 };

	auto frequencies = getFrequencyTable(input);

	auto hufftree = getHuffmanTree(frequencies);

	std::map<char, std::string> encryptions;

	encryptionPreOrder(hufftree->getRoot(), "", encryptions);

	std::ifstream i_file(input);
	std::ofstream o_file(output);

	if (i_file.is_open()) {
		char ch;
		while (i_file.get(ch))
		{
			o_file << encryptions[ch];
			++byte_count;
			bit_count += encryptions[ch].length();
		}
	}

	else throw "Input file was not found";
	i_file.close();
	o_file.close();

	std::ofstream info_file("info_for_decompression.txt");
	for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
	{
		info_file << it->first << '\n' << it->second << '\n';
	}
	info_file.close();
	delete hufftree;

	//dregree of compression is written into the console, along with some interesting information
	std::cout << "--COMPRESSION COMPLETE--\n" << "\nInformation's size:"
		<< "\nBefore compression: " << byte_count * 8 << " bits"
		<< "\nAfter compression: " << bit_count << " bits" << "\nDegree of compression: " << (bit_count * 100) / (byte_count * 8) << "%\n";

}


//reads the frequency table, which was written to file when compressing, needed for decompression
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

//final decompress function
void decompress(bool debug_mode, std::string input, std::string output)
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
				if (debug_mode == 0)
				{
					o_file << current_node->data;
				}
				else
				{
					std::cout << (int)current_node->data << " ";

				}
				current_node = hufftree->getRoot();
			}
		}
	}
	else throw "Input file not found!";
	i_file.close();
	o_file.close();

	//this cout is for aesthetic purposes 
	std::cout << "\n--DECOMPRESSION COMPLETE--";



}




int main(int argc, char** argv)
{
	try {
		bool debug_mode = false;
		char* buf = nullptr;
		size_t sz = 0;
		if (_dupenv_s(&buf, &sz, "DEBUG") == 0 && buf != nullptr)
		{
			debug_mode = buf[0] == '1';
			free(buf);
		}


		bool compress_mode = false;
		bool decompress_mode = false;
		char* input_filename = nullptr;
		char* output_filename = nullptr;

		//6 arguments are needed
		//name of .exe, -c/-d (compression/decompression), -i filename, -o filename)
		if (argc != 6) {
			throw "Incorrect number of arguments!";
		}


		//finding the mode and filenames and assigning them to their corresponding variables
		for (int i = 1; i < argc; i++)
		{
			auto elem = argv[i];
			if (elem[0] != '-') continue;

			if (elem[1] == 'c') compress_mode = true;
			else if (elem[1] == 'd') decompress_mode = true;
			else if (elem[1] == 'i')
			{
				input_filename = argv[++i];
			}
			else if (elem[1] == 'o')
			{
				output_filename = argv[++i];
			}
		}

		//either compress or decompress mode must be true, can't be both
		if ((compress_mode && decompress_mode) || (!compress_mode && !decompress_mode))
			throw "Choose ONE of the following modes:\n compression \n decompression ";
		if (input_filename == nullptr || output_filename == nullptr)
			throw"Input or output filename was not given!";

		if (compress_mode)
		{
			compress(input_filename, output_filename);
		}
		else if (decompress_mode)
		{
			decompress(debug_mode, input_filename, output_filename);
		}
	}

	catch (const char* msg)
	{
		std::cout << msg;
	}
	return 0;
}
