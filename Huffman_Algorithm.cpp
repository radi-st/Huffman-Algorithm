#include <iostream>
#include <fstream>
#include <map>
#include <string>

std::map<char, unsigned int> getFrequencyTable(std::string filename)
{
    std::map<char, unsigned int> frequencies;
    std::ifstream myfile(filename);
    if (myfile.is_open()) {
        while (!myfile.eof())
        {
            char ch;
            myfile.get(ch);
            if (frequencies.find(ch)!= frequencies.end())
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
       data(var_data),frequency(var_frequency), left(var_left), right(var_right) {}

};


int frequencyCompare(HuffNode* rhs, HuffNode* lhs)
{
    if (rhs->frequency > lhs->frequency)
        return 1;
    if (rhs->frequency < lhs->frequency)
        return -1;

    return 0;
}


class HuffTree 
{
private:
    HuffNode* root;
public:
    HuffTree(HuffNode* var_root): root(var_root) {}

};

// 'A':5,  'B':2,  'C':1,  'D':1, 'R': 2

HuffNode* getHuffmanTree(std::map<char, unsigned int> frequencies)
{
    
    unsigned int min_frequency{ 0 };
    char key_min_frequency;
    for (const auto& p : frequencies)
    {
        if (p.second < min_frequency)
        {
            key_min_frequency = p.first;
            min_frequency = p.second;
        }
    }

    HuffTree tree(new HuffNode(min_frequency, key_min_frequency));




    return nullptr;

}



int main()
{
    std::cout << "Hello World!\n";
}
