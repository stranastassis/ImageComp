#include "ppm_format.h"
#include "cpi_format.h"
#include "rle_format.h"
#include "cci_format.h"

#include <iostream>
#include <fstream>

using namespace imaging;
using namespace std;

void interactiveway(string file);
void noninteractiveway(string file1, string file2);
bool fexists(string filename);

Image *image;

int codebook_size = 0;
int block_size = 0;
int threshold = 0;

int main(int argc, char *argv[])
{
	string temp = argv[2];

	if (temp == "-o")
	{
		string source_file = argv[4];
		string dest_file = argv[3];

		for (int i = 5; i < argc; i = i + 2)
		{
			string v = argv[i];
			int j = i;

			if (v == "-b")
				block_size = atoi(argv[++j]);
			else if (v == "-e")
				threshold = atoi(argv[++j]);
			else if (v == "-c")
				codebook_size = atoi(argv[++j]);
			else
			{
				cout << "INVALID ARGUMENT " << endl;
				break;
			}
		}

		noninteractiveway(source_file, dest_file);
	}
	else //main photo.smt [-b][-e][-c]
	{
		/** source image **/
		string file = argv[2]; cout << file << endl;

		/* read all attributes-if any exist.
		   Takes in account that user may
		   have not defined all of them */
		for (int i = 3; i < argc; i = i + 2)
		{
			//cout << argv[i] << endl;
			string v = argv[i];
			int j = i;

			if (v == "-b")
				block_size = atoi(argv[++j]);
			else if (v == "-e")
				threshold = atoi(argv[++j]);
			else if (v == "-c")
				codebook_size = atoi(argv[++j]);
			else
			{
				cout << "INVALID ARGUMENT " << endl;
				break;
			}
		}
		interactiveway(file);
	}
	system("pause");

	return 0;
}

void noninteractiveway(string source, string dest)
{
	//PPM source image
	if (isSuffix(source, "ppm"))
	{
		PPMImageReader reader;
		image = reader.read(source);

		//convert to CPI
		if (isSuffix(dest, "cpi"))
		{
			image->convertToColorPlaneImage();
			CPIImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to RLE
		else if (isSuffix(dest, "rle"))
		{
			image->convertToColorPlaneImage();
			RLEImageWriter writer;
			if(block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			writer.write(dest, *image);
		}
		//convert to CCI
		else if (isSuffix(dest, "cci"))
		{
			image->convertToColorPlaneImage();
			CCIImageWriter writer;
			if (block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			if (codebook_size > 0) writer.setCodeBookLength(codebook_size);
			writer.write(dest, *image);
		}
		else
			cout << "La8os sto deutero orisma";
	}
	//CPI source image
	else if (isSuffix(source, "cpi"))
	{
		CPIImageReader reader;
		image = reader.read(source);

		//convert to PPM
		if (isSuffix(dest, "ppm"))
		{
			image->convertToInterleavedImage();
			PPMImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to RLE
		else if (isSuffix(dest, "rle"))
		{
			RLEImageWriter writer;
			if (block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			writer.write(dest, *image);
		}
		//convert to CCI
		else if (isSuffix(dest, "cci"))
		{
			CCIImageWriter writer;
			if (block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			if (codebook_size > 0) writer.setCodeBookLength(codebook_size);
			writer.write(dest, *image);
		}
		else
			cout << "La8os sto deutero orisma";
	}
	//RLE source Image
	else if (isSuffix(source, "rle"))
	{
		RLEImageReader reader;
		image = reader.read(source);

		//convert to PPM
		if (isSuffix(dest, "ppm"))
		{
			image->convertToInterleavedImage();
			PPMImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to CPI
		else if (isSuffix(dest, "cpi"))
		{
			CPIImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to CCI
		else if (isSuffix(dest, "cci"))
		{
			CCIImageWriter writer;
			if (block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			if (codebook_size > 0) writer.setCodeBookLength(codebook_size);
			writer.write(dest, *image);
		}
		else
			cout << "La8os sto deutero orisma";
	}
	//CCI source Image
	else if (isSuffix(source, "cci"))
	{
		CCIImageReader reader;
		image = reader.read(source);

		//convert to PPM
		if (isSuffix(dest, "ppm"))
		{
			image->convertToInterleavedImage();
			PPMImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to CPI
		else if (isSuffix(dest, "cpi"))
		{
			CPIImageWriter writer;
			writer.write(dest, *image);
		}
		//convert to RLE
		else if (isSuffix(dest, "rle"))
		{
			RLEImageWriter writer;
			if (block_size > 0) writer.setBlockDimension(block_size);
			if (threshold > 0) writer.setThreshold(threshold);
			writer.write(dest, *image);
		}
		else
			cout << "La8os sto deutero orisma";
	}
	else
		cout << "La8os sto tupo twn eikonwn" << endl;

}
void interactiveway(string file)
{
	char answer;
	string temp;
	bool isCCI = false;;

	//PPM source Image
	if (isSuffix(file, "ppm"))
	{
		PPMImageReader reader;
		image = reader.read(file);
		replaceSuffix(file, "cci");
	}
	//CPI source Image
	else if (isSuffix(file, "cpi"))
	{
		CPIImageReader reader;
		image = reader.read(file);
		replaceSuffix(file, "cci");
	}
	//RLE source Image
	else if (isSuffix(file, "rle"))
	{
		RLEImageReader reader;
		image = reader.read(file);
		replaceSuffix(file, "cci");
	}
	//CCI source Image-default convertion to PPM
	else if (isSuffix(file, "cci"))
	{
		CCIImageReader reader;
		image = reader.read(file);
		replaceSuffix(file, "ppm");
		isCCI = true;
	}
	else
	{
		cerr << "Wrong image format.Please choose a ppm,cpi,rle or cci format." << endl;
		return;
	}

	cout << "Save image as " << file << "? (y)es / (n)o" << endl;
	cin >> answer;

	if (answer == 'n' || answer == 'N')
	{
		cout << "Output name: ";
		cin >> file;
		cout << endl;
		if (isCCI)
			file = file + ".ppm";
		else
			file = file + ".cci";
	}

	while (fexists(file))
	{
		char choice;

		cout << "The file exists. Overwrite? (y)es / (n)o";
		cin >> choice;
		if (choice == 'y' || choice == 'Y') break;
		cout << "Output name:";
		cin >> file;
		cout << endl;
		if (isCCI)
			file = file + ".ppm";
		else
			file = file + ".cci";
	}

	if (isCCI)
	{
		PPMImageWriter writer;
		writer.write(file, *image);
	}
	else
	{
		CCIImageWriter writer;
		if (block_size > 0) writer.setBlockDimension(block_size);
		if (threshold > 0) writer.setThreshold(threshold);
		if (codebook_size > 0) writer.setCodeBookLength(codebook_size);
		writer.write(file, *image);
	}
	cout << "The image was successfully written to " << file << endl;
}

bool fexists(string filename)
{
	ifstream ifile(filename);
	if (ifile)
		return true;
	else
		return false;
}
