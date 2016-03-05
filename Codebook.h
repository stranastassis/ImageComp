#include "Image.h"
#include "Block.h"


namespace imaging{

	class Codebook{
	protected:
		size_t size;
		Component error_margin;
		unsigned int counter;
	public:
		/*Enas constructor p bazei gia mege8os
		auto p tou pername gia error bainei 
		h default thmh dld 6 */
		Codebook(const size_t & codebook_size);
		//size accessor
		size_t getSize() const;
		// error seter
		void setErrorMargin(Component err);
		/*Psaxnei to block p tou pername 
		an to brei gurizei to code tou 
		h an brei to antistrofo epistrefei
		to arnhtiko code.An dn uparxei prosti8etai
		kai gurizei to code p pire
		*/
		int addBlock(const Block & block);
		/*Psaxnei gia to block an to brei girizei
		true alliws false.Prosoxh! dn kanei elegxo
		gia to antistrofo
		*/
		bool find(const Block &block);
		/*Epistrefei to block p bike 
		sumfwna me thn seira tou.Dld 
		otan to orisma p pername einai 3
		epistrefei to trito ktlp.
		Prosoxh!to orisma prepei na einai
		>1,An do8ei arnhtiko orisma epistrefetai
		to prwto,an do8ei orisma megalutero tou
		mege8ous tou Codebook epistrefetai to teleutaio*/
		Block & getBlock(int posis);
	};
	class equal{
		public:
		bool operator()(const Block & block1,const Block & block2){
		return block1==block2;
		}
	};
}