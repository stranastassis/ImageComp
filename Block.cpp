#include "Block.h"

#include <iostream>

namespace imaging {


	Block::Block(const size_t & block_size):size(block_size),error_margin(0)
	{
		data = new Component[size];
	}
	Block::Block(const Block & src):size(src.getSize()),error_margin(src.error_margin)
	{
		
		std::cout << "copy constructor called" << std::endl;
		data=new Component(size);
		
	
		memcpy(data,src.data,size*sizeof(Component));
	}
	Block * Block::copyFromImage(Image & src, Image::channel_t channel, const ivec2 & pos, const size_t & size){

		std::cout << "i am in" << std::endl;

		if (size == 0 || pos.x > src.getHeight() - 1 || pos.y > src.getWidth() - 1)
		{
			std::cout << "i am in bad if" << std::endl;
			return nullptr;
		}
			
		if(!src.isInterleaved())
			src.convertToColorPlaneImage();
		int pointer;//Auto to pedio mas boh8aei na broume thn prwth 8esh apo thn opoia prepei na arxisoume na pernoume Components
		if (channel == 0)
		{
			std::cout << "i am in red" << std::endl;
			pointer=(pos.x-1)*src.getWidth()+(pos.y-1);
		}
		else if(channel==2)
			pointer=(pos.x-1)*src.getWidth()+(pos.y-1)+2*src.getWidth()*src.getHeight();
		else
			pointer=(pos.x-1)*src.getWidth()+(pos.y-1)+src.getWidth()*src.getHeight();

		int realsize=size > src.getWidth() - pos.x?src.getWidth() - pos.x:size; //To pragmatiko mege8os tou block
		std::cout << realsize << std::endl;

		Block* block(new Block(realsize));//=new Block(realsize);

		Component * temp=new Component[realsize];

		Component *data = src.getRawDataPtr(); std::cout << "image data read into *data" << std::endl;

		for(int i=0;i<realsize;i++)//<=
		{
			temp[i]=data[pointer + i];
		}
		std::cout << "block read" << std::endl;

		block->setData(temp);//data
		std::cout << "set data" << std::endl;

		delete[] temp; 
		std::cout << "temp deleted" << std::endl;

		return block;	
	}

	Block::~Block(){
		delete[] data;
	}
	size_t Block::getSize() const{
		return size;
	}
	Component * Block::getDataPtr() const{
		return data;
	}
	void Block::setData(const Component * src){
		memcpy(data,src,size*sizeof(Component));
	}
	Block Block::reverse() const{
		Block tempreversedblock=Block(size);
		Component * temp1=new Component[size];
		for(unsigned  int i=0;i<size;i++){
			temp1[i]=data[size-1-i];
		}
		tempreversedblock.setData(data);
		delete [] temp1;
		return tempreversedblock;
	}
	void Block::setErrorMargin(Component err){
		error_margin=err<=0?0:err;
	}
	Component & Block::operator[] (const size_t index) const{
		return data[index];
	}
	Component & Block::operator() (const size_t & index) const{
		int i= index>size-1?size-1:index<0?size-1:index;
		return data[i];
	}
	Block & Block::operator=(const Block & src){
		if(&src==this){
			return *this;
		}
		if(data!=nullptr){
			delete [] data;
		}
		size=src.size;
		error_margin=src.error_margin;
		memcpy(data,src.data,size*sizeof(Component));
		return *this;
	}
	bool Block::operator==(const Block& rhs) const{
		Component * temp=rhs.getDataPtr();
		if(error_margin==0){
			for(int i=0;size<size && size<rhs.getSize();i++){
				if(data[i]=!temp[i])
					return false;
			}
		}
		for(int i=0;size<size && size<rhs.getSize();i++){
			if(abs(data[i]-temp[i])>error_margin)
					return false;
		}
		return true;

	}
	bool Block::operator!=(const Block& rhs) const{
		return(!(rhs==*this));
	}
}	