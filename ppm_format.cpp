#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ppm_format.h"
#include <iostream>
// Insert implementation code for the PPMImageWriter and PPMImageReader classes here
using namespace std;

namespace imaging
{
	void set(char temp, FILE *infile, unsigned int &width);

	void PPMImageWriter::write(string filename, const Image & src)
	{
		// file name from string to char so // 
		// it can be used in fopen function //
		const char* name = filename.c_str();

		// VARIABLES //
		FILE *outfile;								// pointer to file
		unsigned int maxcolor = 255;				// maximum pixel value
		unsigned int width = src.getWidth();		// image width
		unsigned int height = src.getHeight();		// image height
		string entry;								// message to logger

		// open binary file in write mode //
		outfile = fopen(name, "wb");

		// check for error while opening file //
		if (!outfile)
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

		//Write PPM header
		fprintf(outfile, "%2s", "P6\n");					// write P6 identifier
		fprintf(outfile, "%u %u\n", width, height);			// write width - height
		fprintf(outfile, "%u\n", maxcolor);					// write maxcolor value
        
		entry = string("Done writing header");
		addLogEntry(entry);

		entry = string("Writing image data. . .");
		addLogEntry(entry);

		// writeBuffer will hold the image data //
		Component *writeBuffer = new Component[3 * height*width];

		// obtain image's data //
		writeBuffer = src.getRawDataPtr();

		// write image data to file //
		fwrite(writeBuffer, sizeof(Component), 3 * height*width, outfile);

		entry = string("Done writing image data. . .");
		addLogEntry(entry);

		// de-allocate memory //
		delete[] writeBuffer;

		// close file //
		fclose(outfile);

		destroyLogger();
	}

	Image* PPMImageReader::read(std::string filename)
	{
		// Open logging file //
		createLogger("LOGFILE");

		// file name from string to char so // 
		// it can be used in fopen function //
		const char *name = filename.c_str();

		// VARIABLES //
		unsigned char id[3];				// P6 identifier
		unsigned int width;					// image width
		unsigned int height;				// image height
		unsigned int maxcolor;				// maxcolor value
		char temp;							// current character 
		FILE *infile;						// pointer to file
		string entry;						// message to logger
		
		// open binary file in read mode //
		infile = fopen(name, "rb");

		// check for error while opening file //
		if (!infile)
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
		fscanf(infile, "%2s", id);

		// check for valid id //
		if (!((id[0] == 'P') && (id[1] == '6')))
		{
			// message to console //
			cout << "Invalid identifier.Expected P6. . ." << endl;
			// message to logger //
			entry = string("Invalid identifier.Expected P6. . .");
			addLogEntry(entry);
			fclose(infile);
			return 0;
		}

		// read next char //
		temp = fgetc(infile);

		// locate width and read its value //
		set(temp, infile, width);

		// check valid width value //
		if (!(width > 0))
		{
			// message to console //
			cout << "Invalid width value.Width should be a positive value." << endl;
			// message to logger //
			entry = string("Invalid width value.Width should be a positive value.");
			addLogEntry(entry);
			fclose(infile);
			return 0;
		}
			
		// read next char //
		temp = fgetc(infile);

		// locate height and read its value //
		set(temp, infile, height);

		// check valid height value //
		if (!(height > 0))
		{
			// message to console //
			cout << "Invalid height value.Height should be a positive value." << endl;
			// message to logger //
			entry = std::string("Invalid height value.Height should be a positive value.");
			addLogEntry(entry);
			fclose(infile);
			return 0;
		}
		
		// read next char //
		temp = fgetc(infile);

		// locate maxcolor and read its value //
		set(temp, infile, maxcolor);

		// check valid maxcolor value //
		if ((maxcolor > 255) || (maxcolor < 0))
		{
			// message to console //
			cout << "Invalid maxcolor value." << endl;
			// message to logger //
			entry = string("Invalid maxcolor value.");
			addLogEntry(entry);
			fclose(infile);
			return 0;
		}
	
		// read next char //
		temp = fgetc(infile);

		if ((temp == ' ') || (temp == '\n'))
		{
			entry = string("Header is in valid format. . .");
			addLogEntry(entry);

			// tempbuf holds the image data // 
			Component *tempbuf = new Component[3 * height*width];

			entry = string("Reading image data. . .");
			addLogEntry(entry);

			// read image data //
			fread(tempbuf, sizeof(Component), 3 * width*height, infile); //cout << "read data" << endl;

			entry = string("Done reading image data.");
			addLogEntry(entry);

			//Image object
			Image *image = new Image(width, height, tempbuf, true);

			delete[] tempbuf;
			//return image Object
			return image;
		
			
			
		}
		else
		{
			// message to console //
			cout << "Header is in invalid format. . ." << endl;
			// message to logger //
			entry = string("Header is in invalid format. . .");
			addLogEntry(entry);
			fclose(infile);
			return 0;
		}

		//close file
		fclose(infile);
	}

	void set(char temp, FILE *infile, unsigned int &value)
	{

		//if end of line is reached read first char of new line
		if (temp == '\n')
			temp = fgetc(infile);

		//if char is whitespace continue reading 
		while (temp == ' ')
		{
			temp = fgetc(infile);
			if (temp == '\n')
				break;
		}

		//skip comments
		while (1)
		{
			if (temp == '#')
			{
				while (temp != '\n')
					temp = fgetc(infile);
				temp = fgetc(infile);
			}
			else
			{
				ungetc(temp, infile); 
				break;
			}
		}

		//assign value
		fscanf(infile, "%d", &value); 
	}
}