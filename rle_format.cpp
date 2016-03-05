#include "rle_format.h"
#include "Block.h"
#include <iostream>
#include <vector>
using namespace std;

namespace imaging
{
	void RLEImageWriter::write(string filename, const Image & src)
	{
		//filename from string to char//
		const char* file = filename.c_str();

		//variables//
		FILE *fp;
		unsigned char id[3] = {'C','P','I'};
		unsigned char version = 2;
		unsigned short endian = 258;
		unsigned short width = src.getWidth();
		unsigned short height = src.getHeight();
		unsigned short b_size = block_length;
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

		entry = string("Done writing header");
		addLogEntry(entry);

		entry = string("Writing image data. . .");
		addLogEntry(entry);


		///////////////////////////////////////////
		/// encoding happens here				///
		/// data are divided to blocks			///
		/// depending on block size.			///
		/// error is considered for encoding	///
		/// encoded data are stored to file		///
		///////////////////////////////////////////

		//number of blocks
		unsigned int n_blocks = (int)ceil(width / (float)b_size);
		//number of data in last block
		unsigned int b_last = width - (n_blocks - 1)*b_size;
		Component* tempdata;
		Image* tempImage=new Image(src);
		tempImage->convertToColorPlaneImage();
		tempdata=tempImage->getRawDataPtr();
		unsigned int flag=0;
		unsigned int counter=1;
		Component count;
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
				int c=0;
				unsigned int flag2;
			while(c<b_size){
				count=1;
				flag2=c;
				c++;
				while(c<b_size && count<=255 && abs(NormalBlock->operator()(flag2)-NormalBlock->operator()(c))<=threshold){
					count=count+1;
					c++;
				}
				//cout<<NormalBlock(flag2);
				fwrite(&NormalBlock->operator()(flag2),sizeof(Component),sizeof(NormalBlock->operator()(flag2)),fp);
				
				//writebuffer.push_back(NormalBlock(flag2));
				//cout<<writebuffer[0]<<endl;
				//writebuffer.push_back(count);
				fwrite(&count,sizeof(Component),sizeof(count),fp);
				//cout<<writebuffer[1]<<endl;
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
				unsigned int c=0;
				unsigned int flag2;
				while(c<b_last){
				 count=1;
				 flag2=c;
				c++;
				while(c<b_last && count<=255 && abs(LastBlock->operator()(flag2)-LastBlock->operator()(c))<=threshold){
					count=count+1;
					c++;
				}
				//writebuffer.push_back(NormalBlock(flag2));
				fwrite(&LastBlock->operator()(flag2),sizeof(Component),sizeof(LastBlock->operator()(flag2)),fp);
				//writebuffer.push_back(count);
				fwrite(&count,sizeof(Component),sizeof(LastBlock->operator()(flag2)),fp);
			}
		}
		}
	
		/*for (unsigned  int i = 0; i < n_blocks*height * 3; i++)
		{
			pos.x=i/src.getWidth()+1;
			Image::channel_t channel;
			if(i<n_blocks*height)
				channel=Image::RED;
			else if(i<2*n_blocks*height)
				channel=Image::GREEN;
			else 
				channel=Image::BLUE;
			//pos.y= pos.y+b_size>src.getWidth()-1?0:pos.y+b_size;
			pos.y=i%src.getWidth()*(b_size/3)+1;
			block=Block::copyFromImage(const_cast<Image &>(src),channel,pos,b_size);
			int j=0;
			while(j<b_size){
				Component count=1;
				Component val=block->operator()(j);
				j++;
				while(j<b_size && count<=255 && abs(val-block->operator()(j))<=threshold){
					count=count+1;
					j++;
				}
				writebuffer.push_back(val);
				writebuffer.push_back(count);
			}
		}
		*/
		//cout<<"Write";
		//fwrite(writebuffer->data(), sizeof(Component), writebuffer->size(), fp);

		entry = string("Done writing image data. . .");
		addLogEntry(entry);

		
		//de-allocate memory
		//delete tempdata;
		//close file
		delete tempImage;
		fclose(fp);

		//destroyLogger();
		
	}

	Image * RLEImageReader::read(string filename)
	{
		//open logging file//
		createLogger("LOGFILE");

		//filename from string to char//
		const char* file = filename.c_str();

		//variables//
		FILE *fp;							// pointer to file
		unsigned char id[3];				// CPI identifier
		unsigned char version;				// version number(2 at this case)
		unsigned short endian;				//
		unsigned short width;				// image width
		unsigned short height;				// image height
		unsigned short b_size;				// maximum block size
		string entry;						// message to logger

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

		//check for valid version(2 for rle format)//
		if (!(version == 2))
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
		fread(&width, sizeof(unsigned short), 1, fp);							//read image width

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

		fread(&b_size, sizeof(unsigned short), 1, fp);							//read maximum block size

		//read data//
		Component *buffer = new Component[3 * width*height];
		Component temp[2];
		int readedBytes=0;
		while(readedBytes<3*width*height){
			fread(temp, sizeof(Component),2, fp);
			int counter=1;
			while(counter<=temp[1] && readedBytes<3*width*height){
				buffer[readedBytes]=temp[0];
				counter++;
				readedBytes++;
			}
		}

		//Image Object
		Image *image = new Image(width, height, buffer);
		//delete array
		delete[]buffer;
		
		//close file
		fclose(fp);
		return image;



	}
}