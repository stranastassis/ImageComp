#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "cpi_format.h"
#include "Image.h"
#include<iostream>
#include<string>
// Insert implementation code for the CPImageWriter and CPIImageReader classes here
using namespace std;

namespace imaging
{
	void CPIImageWriter::write(std::string filename, const Image &src)
	{
		// file name from string to char so // 
		// it can be used in fopen function //
		const char*newFile = filename.c_str();

		// VARIABLES //
		unsigned char id[] = { 'C','P','I' };			// CPI identifier
		unsigned char version = 1;						// version number(1 at this case)
		unsigned short endian = 258;					// endian value
		unsigned short width = src.getWidth();			// image width
		unsigned short height = src.getHeight();		// image height
		string entry;								    // message to logger
		FILE *nfile;									// pointer to file

		// open binary file in write mode //
		nfile = fopen(newFile, "wb"); 

		// check for error while opening file //
		if (!nfile)
		{
			// message to console //
			cout << "Error opening file " << filename << ". . ." << endl;
			// message to logger //
			entry = string("Error opening file ") + filename + ". . .";
			addLogEntry(entry);
			return;
		}

		// if no error occurs //
		// message to console //
		cout << "file " << filename << " opened" << endl;
		// message to logger //
		entry = string("file ") + filename + " opened";
		addLogEntry(entry);

		entry = string("Writing Header. . .");
		addLogEntry(entry);

		//write CPI
		fwrite(id, sizeof(unsigned char), sizeof(id), nfile);
		//write 1
		fwrite(&version, sizeof(unsigned char), 1 , nfile);
		//write 258
		fwrite(&endian, sizeof(unsigned short), 1, nfile);
		//write width
		fwrite(&width, sizeof(unsigned short), 1, nfile);
		//write height
		fwrite(&height, sizeof(unsigned short), 1, nfile);

		entry = string("Done writing header");
		addLogEntry(entry);
		
		entry = string("Writing image data. . .");
		addLogEntry(entry);

		// writeBuffer will hold the image data //
		Component *writeBuffer = new Component[3 * height*width]; 

		// obtain image's data //
		writeBuffer = src.getRawDataPtr(); 

		// write image data to file //
		fwrite(writeBuffer, sizeof(Component), 3 * height*width, nfile); 
		cout << "done writing data" << endl;

		entry = string("Done writing image data");
		addLogEntry(entry);

		// de-allocate memory //
		delete[] writeBuffer;
		
		// close file //
		fclose(nfile);

		destroyLogger();
	}

	Image * CPIImageReader::read(std::string filename)
	{
		// Open logging file //
		createLogger("LOGFILE");

		// file name from string to char so // 
		// it can be used in fopen function //
		const char* namefile = filename.c_str();

		// VARIABLES //
		FILE *fp;						// pointer to file
		unsigned char id[3];			// CPI identifier
		unsigned char ver;				// version number(1 at this case)
		unsigned short value;			// endian value
		unsigned short width;			// image width 
		unsigned short height;          // image height
		string entry;					// message to logger

		// open binary file in read mode //
		fp = fopen(namefile, "rb");

		// check for error while opening file //
		if (!fp)
		{
			// message to console //
			cout << "Error opening file " << filename << ". . ." << endl;
			// message to logger //
			entry = string("Error opening file ") + filename + ". . .";
			addLogEntry(entry);
			return 0;
		}
		
		// if no error occurs //
		// message to console //
		cout << "file " << filename << " opened" << endl;
		// message to logger //
		entry = string("file ") + filename + " opened";
		addLogEntry(entry);

		entry = string("Reading Header. . .");
		addLogEntry(entry);

		// read id //	
		fread(id, sizeof(unsigned char), sizeof(id), fp);

		// check for valid id //
		if (!((id[0] == 'C') && (id[1] == 'P') && (id[2] == 'I')))
		{
			// message to console //
			cout << "Invalid identifier.Expected CPI. . ." << endl;
			// message to logger //
			entry = string("Invalid identifier.Expected CPI. . .");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}
			
		// read version //
		fread(&ver, sizeof(unsigned char), 1, fp);

		// check for number 1 as version number //
		if (!(ver == 1))
		{
			// message to console //
			cout << "Invalid version. . ." << endl;
			// message to logger //
			entry = string("Invalid version. . .");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}
	
		// read 258 //
		fread(&value,sizeof(value), 1, fp);

		// read width //
		fread(&width, sizeof(unsigned short), 1, fp); 

		// check width value //
		if (!(width > 0))
		{
			// message to console //
			cout << "Invalid width value.Width should be a positive value." << endl;
			// message to logger //
			entry = string("Invalid width value.Width should be a positive value.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}

		// read height //
		fread(&height, sizeof(unsigned short), 1, fp); 

		// check height value //
		if (!(height > 0))
		{
			// message to console //
			cout << "Invalid height value.Height should be a positive value." << endl;
			// message to logger //
			entry = std::string("Invalid height value.Height should be a positive value.");
			addLogEntry(entry);
			fclose(fp);
			return 0;
		}

		// buffer will hold the image data //
		Component *buffer = new Component[3 * width*height]; 

		entry = string("Reading image data. . .");
		addLogEntry(entry);

		// read image data //
		fread(buffer, sizeof(Component), 3 * width*height, fp);

		entry = string("Done reading image data.");
		addLogEntry(entry);

		//Image Object
		Image *image = new Image(width, height, buffer);
		//de-allocate memory
		delete[]buffer;	
		//close file
		fclose(fp);
		//return image Object
		return image;



	}
}
