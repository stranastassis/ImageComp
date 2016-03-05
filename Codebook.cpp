#include <unordered_map>
#include <string>
#include "Codebook.h"
#include "cci_format.h"
#include <iostream>

using namespace std;
namespace imaging{
		std::vector<Block> pos;
		std::unordered_map<Block,unsigned int,BlockHasher,equal> book;
		std::unordered_map<Block,unsigned int,BlockHasher,equal>::iterator code;
	Codebook::Codebook(const size_t & codebook_size):size(codebook_size),counter(0),error_margin(6){	
	}
	size_t Codebook::getSize() const{
		return size;
	}
	void Codebook::setErrorMargin(Component err){
		error_margin=err;
	}
	int Codebook::addBlock(const  Block & block){
		Block tempblock=Block(block.getSize());
		tempblock.setData(block.getDataPtr());
		tempblock.setErrorMargin(error_margin);
		if(find(tempblock)){
			return code->second;
		}
		else if(find(block.reverse())){
			int temp=code->second;
			return temp*(-1);
		}
		
		else{
			if(counter<size){
				book[tempblock]=++counter;
				pos.push_back(tempblock);
				return counter;
			}
			else{
				bool flag=true;
				unsigned int error_counter=1;
				while(flag){
					tempblock.setErrorMargin(error_margin+error_counter);
					if(find(tempblock)){
						return code->second;
					}
					else if(find(tempblock.reverse())){
					int temp=code->second;
					return temp*(-1);
					}
				}
			}
		}

	}
	bool Codebook::find(const Block & block){
		code=book.find(block);
		if(code==book.end())
			return false;
		else
			return true;
	}
	Block & Codebook::getBlock(int posis){
		if(posis<0){
			return pos[0];
		}
		else if(posis>size){
			return pos[size-1];
		}
		else{
			return pos[posis-1];
		}
	}
}
