#include <iostream>
#include <fstream>

using namespace std;

int main( int argc, char* argv[] )
{
	
	unsigned int side = 30;
	unsigned int dim = side*side;

	string filename = argv[1];

	std::fstream file(filename);
	double a[dim];

	int counter =0;

	while (!file.eof()) {
		for (int i = 0; i < dim; ++i)
		{
			file>> a[i];
		}

		for (int i = 0; i < side; ++i)
		{
			for (int j= 0; j < side; ++j)
			{
				if (a[i*side + j] > 0.5f)
					std::cout << "\033[1;" << 31 << "m 0\033[0m";
				else
					std::cout << "\033[1;" << 37 << "m 1\033[0m";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;

		++counter;
	}
	
	std::cout<<"Total Images: " << counter-1 << std::endl;
	system("pwd");
	std::cout<< filename << std::endl;

	return 0;
}
