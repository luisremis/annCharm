#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main( void )
{
	string filename = "train_imgs_0_1_900dim.mat";
	std::fstream file(filename.c_str());

	std::vector<std::string> vec;

	int counter =0;

	while (!file.eof()) {

		std::string aux;
		getline(file, aux);
		vec.push_back(aux);
	}

	std::fstream newfile("train_imgs_0_1_900dimMixed20.mat");
	std::fstream newfilelabel("train_labels_0_1_900dimMixed20.mat");

	int half = 100;

	for (int k = 0; k < 10; ++k)
	{
		/* code */
		for (int i = 0; i < 5; ++i)
		{
			int tw = 20*i;
			for (int j = 0; j < 20; ++j)
			{
				newfile << vec[tw + j] 		<< std::endl;
				newfilelabel << "0" << std::endl;
			}
			for (int j = 0; j < 20; ++j)
			{
				newfile << vec[tw + j +half]	<< std::endl;
				newfilelabel << "1" << std::endl;
			}
		}
	}
/*
	for (int i = 0; i + half < 200 ; ++i)
	{
		newfile << vec[i] 		<< std::endl;
		newfile << vec[i+half]	<< std::endl;
		newfilelabel << "0" << std::endl;
		newfilelabel << "1" << std::endl;
	}
	for (int i = 0; i + half < 200 ; ++i)
	{
		newfile << vec[i] 		<< std::endl;
		newfile << vec[i+half]	<< std::endl;
		newfilelabel << "0" << std::endl;
		newfilelabel << "1" << std::endl;
	}
*/	
	return 0;
}