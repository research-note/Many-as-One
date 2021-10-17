#include "main/Parser.hpp"
#include <iostream>

using namespace std;

int main(void)
{
	Parser parser;
    
    // mnist
	parser.loadMnist();

    cout<<fixed;
    cout.precision(1);
    for (int i=0; i<28; ++i) {
        for (int j=0; j<28; ++j) {
            cout << test_image[0][(i * 28) + j] << " ";
        }
        cout << endl;
    }


    // // png
    // parser.loadPng("./png_data/fruits.png");

	return 0;
}
