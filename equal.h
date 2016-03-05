#include "Block.h"


namespace imaging{
		class equal{
	public:
		bool operator()(const Block & block1,const Block & block2){
		return block1==block2;
		}
	};
}