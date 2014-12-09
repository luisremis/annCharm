#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

int main( void )
{
	string fileimgs   = "train_imgs_all_900dim.mat";
	string filelabels = "train_labels_all_900dim.mat";
	std::fstream filei(fileimgs.c_str());
	std::fstream filel(filelabels.c_str());

	std::vector< std::vector<std::string> > imgs;
	std::vector<int> labels;

	std::map<int, int> map;

	for (int i = 0; i < 10; ++i)
	{
		std::vector<std::string> aux;
		imgs.push_back( aux );
	}

	int counter =0;

	while (!filei.eof()) {

		std::string aux;
		int num;
		getline(filei, aux);
		filel >> num;
		map[num]++;

		imgs.at(num).push_back(aux);
	}

	for (int i = 0; i < 10; ++i)
	{
		std::cout << map[i] << std::endl;
	}


	std::ofstream newfile("train_imgs_all_900dimMixed30.mat");
	std::ofstream newfilelabel("train_labels_all_900dimMixed30.mat");

	int half = 100;

	for (int k = 0; k < 150; ++k)
	{
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 30; ++j)
			{
				newfile << imgs.at(i).at(i*30 + j) << std::endl;
				newfilelabel << i << std::endl;
			}
		}
	}

	return 0;
}