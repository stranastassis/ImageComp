//------------------------------------------------------------
//
// C++ course assignment code 
//
// G. Papaioannou, 2013
//
//
//-------------------------------------------------------------

#ifndef _CCI
#define _CCI

#include "rle_format.h"
#include "Block.h"
namespace imaging
{
	

	class BlockHasher 
	{
	public:
		size_t operator () (const Block & block)
		{
			size_t sz = block.getSize();
			size_t hashcode = 0;
			for (size_t i=0; i<sz/2; i++)
			{
				hashcode += ((size_t)block[i]/8 + (size_t)block[sz-1-i]/8+13) << i;
			}
			if (sz%2!=0)
				hashcode += ((size_t)block[sz/2]/4+13) << sz/2;
			return hashcode;
		}
	};

	
	class CCIImageWriter : public RLEImageWriter
	{
	protected: 
		unsigned short codebook_length;
	public:
		virtual void write(std::string filename, const Image & src);
		CCIImageWriter() : RLEImageWriter("cci"), codebook_length(4096) 
		{
			block_length = 4;
		}
		void setCodeBookLength(unsigned short len);
	};


	class CCIImageReader : public RLEImageReader
	{
		public:
			virtual Image * read(std::string filename);
			CCIImageReader() 
				: RLEImageReader("CCI") {}
	};

} //namespace imaging

#endif