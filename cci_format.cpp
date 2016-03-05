#include "cci_format.h"
#include <iostream>
#include "Codebook.h"
#include <unordered_map>

using namespace std;

namespace imaging 
{
	void CCIImageWriter::write(string filename,const Image &src){
		//filename from string to char//
		const char* file = filename.c_str();

		//variables//
		FILE *fp;
		unsigned char id[3] = {'C','P','I'};
		unsigned char version = 3;
		unsigned short endian = 258;
		unsigned short width = src.getWidth();
		unsigned short height = src.getHeight();
		unsigned short b_size = 8;
		string entry;

		//open binary file in write mode//
		fp = fopen(file, "wb");

		// check for error while opening //
		// set logger entry				 //
		if (!fp)
		{
			//message to console//
			cerr << "Error opening file. . ." << endl;
			//message to logger//
			entry = string("Error opening file ") + file;
			addLogEntry(entry);
			return;
		}
		else
		{
			entry = string("file ") + file + " opened";
			addLogEntry(entry);
		}

		entry = string("Writing Header. . .");
		addLogEntry(entry);

		//write CPI
		fwrite(id, sizeof(unsigned char), sizeof(id), fp);
		//write 1
		fwrite(&version, sizeof(unsigned char), 1, fp);
		//write 258
		fwrite(&endian, sizeof(unsigned short), 1, fp);
		//write width and height
		fwrite(&width, sizeof(unsigned short), 1, fp);
		fwrite(&height, sizeof(unsigned short), 1, fp);
		//write maximum block size
		fwrite(&b_size, sizeof(unsigned short), 1, fp);
		//write codebook size
		fwrite(&codebook_length,sizeof(unsigned short),1,fp);

		entry = string("Done writing h eader");
		addLogEntry(entry);
		Codebook codebook=Codebook(codebook_length);
		unsigned int n_blocks = (int)ceil(width / b_size);
		//number of data in last block
		unsigned int b_last = width - (n_blocks - 1)*b_size;
		Component* tempdata;
		Image* tempImage=new Image(src);
		tempImage->convertToColorPlaneImage();
		tempdata=tempImage->getRawDataPtr();
		unsigned int flag=0;
		unsigned int counter=1;
		int flagCodebook=0;
		while(flagCodebook<codebook_length){
			Component *data=new Component[b_size];
			Block NormalBlock=Block(b_size);
			NormalBlock.setErrorMargin(threshold);
			if(counter!=n_blocks){	
				for(unsigned int j=0;j<b_size;j++){
					data[j]=tempdata[flag+j];
					cout<<data[j]<<endl;
				}
				NormalBlock.setData(data);
			
				cout<<NormalBlock[0]<<endl<<NormalBlock[1]<<endl<<NormalBlock[2]<<endl<<NormalBlock[3]<<endl;
				//delete NormalBlock;
				delete [] data;
				counter++;
				flag=flag+b_size;
				
				if(flagCodebook<codebook.addBlock(NormalBlock)) {
					flagCodebook++;
				}
			}
			else{
				Component *data2=new Component[b_last];
				Block *LastBlock=new Block(b_last);
				LastBlock->setErrorMargin(threshold);
				for(unsigned int j=0;j<b_last;j++){
					data2[j]=tempdata[flag+j];
				}
				LastBlock->setData(data2);
				delete [] data2;
				counter=1;
				flag=flag+b_last;
				if(flagCodebook<codebook.addBlock(*LastBlock) ){
					flagCodebook++;
				}
		}
		}
		//write codebook
		for(int i=1;i<=codebook_length;i++){
			size_t size=codebook.getBlock(i).getSize();
			fwrite(&size,sizeof(size_t),1,fp);
			fwrite(codebook.getBlock(i).getDataPtr(),sizeof(Component),size,fp);
		}
		//write data
		flag=0;
		counter=1;
		short code;
		for(unsigned int i=0;i<n_blocks*height * 3;i++){
			Component *data=new Component[b_size];
			Block *NormalBlock=new Block(b_size);
			NormalBlock->setErrorMargin(threshold);
			if(counter!=n_blocks){	
				for(unsigned int j=0;j<b_size;j++){
					data[j]=tempdata[flag+j];
				}
				NormalBlock->setData(data);
				//delete NormalBlock;
				delete [] data;
				counter++;
				flag=flag+b_size;
				//code=codebook.addBlock(*NormalBlock);
				fwrite(&code,sizeof(short),1,fp);

			}
			else{
				Component *data2=new Component[b_last];
				Block *LastBlock=new Block(b_last);
				LastBlock->setErrorMargin(threshold);
				for(unsigned int j=0;j<b_last;j++){
					data2[j]=tempdata[flag+j];
				}
				LastBlock->setData(data2);
				delete [] data2;
				counter=1;
				flag=flag+b_last;
			//	code=codebook.addBlock(*LastBlock);
				fwrite(&code,sizeof(short),1,fp);
		}
		entry = string("Done writing image data. . .");
		addLogEntry(entry);
		delete tempImage;
		fclose(fp);
	}
	}
	Image* CCIImageReader::read(string filename){
		//open logging file//
		createLogger("LOGFILE");

		//filename from string to char//
		const char* file = filename.c_str();

		//variables//
		FILE *fp;							// pointer to file
		unsigned char id[3];				// CPI identifier
		unsigned char version;				// version number(3 at this case)
		unsigned short endian;				//
		unsigned short width;				// image width
		unsigned short height;				// image height
		unsigned short b_size;				// maximum block size
		string entry;	// message to logger
		unsigned short codebook_length; //codebook size

		// open binary file in read mode //
		fp = fopen(file, "rb");

		// check for error while opening //
		// set logger entry				 //
		if (!fp)
		{
			//message to console//
			cerr << "Error opening file. . ." << endl;
			//message to logger//
			entry = string("Error opening file ") + file;
			addLogEntry(entry);
			return 0;
		}
		else
		{
			entry = string("file ") + file + " opened";
			addLogEntry(entry);
		}

		//read header//
		entry = string("Reading Header. . .");
		addLogEntry(entry);
				fread(id, sizeof(unsigned char), sizeof(id), fp);						//read identifier(CPI)

		//check for valid id(CPI at this case)//
		//if id is invalid an error message is//
		//shown and application terminates    //
		if (!(id[0] == 'C' && id[1] == 'P' && id[2] == 'I'))
		{
			//message to console
			cerr << "Invalid identifier. . .Expected 'CPI'." << endl;
			//message to logger
			entry = string("Invalid identifier. . .Expected 'CPI'.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}
		fread(&version, sizeof(unsigned char), 1, fp);							//read version

		//check for valid version(3 for cci format)//
		if (!(version == 3))
		{
			//message to console//
			cerr << "Invalid version number. . .Expected 2." << endl;
			//message to logger//
			entry = string("Invalid version number. . .Expected 2.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}
		fread(&endian, sizeof(unsigned short), 1, fp);							//read endian number
		fread(&width, sizeof(unsigned short), 1, fp);
				//check image width to be a positive number//
		if (!(width > 0))
		{
			//message to console//
			cerr << "Invalid width field. . .Expected positive number." << endl;
			//message to logger//
			entry = string("Invalid width field. . .Expected positive number.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}

		fread(&height, sizeof(unsigned short), 1, fp);							//read image height

		//check image height to be a positive number//
		if (!(height > 0))
		{
			//message to console//
			cerr << "Invalid height field. . .Expected positive number." << endl;
			//message to logger//
			entry = string("Invalid height field. . .Expected positive number.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}
		fread(&b_size, sizeof(unsigned short), 1, fp);	
		fread(&codebook_length,sizeof(unsigned short),1,fp);
		Codebook codebook=Codebook(codebook_length);
		//read codebook
		for(int i=0;i<codebook_length;i++){
			size_t blocksize;
			fread(&blocksize,sizeof(size_t),1,fp);
			Component * data=new Component[blocksize];
			fread(data,sizeof(Component),blocksize,fp);
			Block block=Block(blocksize);
			block.setData(data);
			codebook.addBlock(block);
			delete [] data;
		}
		Component* Imagedata=new Component[3*width*height];
		int readedBytes = 0;
		short code;
		//read data
		while(readedBytes<3*width*height){
			fread(&code,sizeof(short),1,fp);
			if(code<0){
				Block temp_block=Block(codebook.getBlock((-1)*code));
				temp_block.reverse();
				for(int j=0;j<temp_block.getSize();j++){
					Imagedata[readedBytes+j]=temp_block[j];
				}
				readedBytes=readedBytes+temp_block.getSize();
			}
			else{
				Block temp_block=Block(codebook.getBlock(code));
				for(int j=0;j<temp_block.getSize();j++){
					Imagedata[readedBytes+j]=temp_block[j];
				}
				readedBytes=readedBytes+temp_block.getSize();
			}
		}
	Image *image=new Image(width,height,Imagedata);
	delete [] Imagedata;
	return image;
	}
	void CCIImageWriter::setCodeBookLength(unsigned short len){
		block_length=len;
	}

}

	