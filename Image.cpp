//------------------------------------------------------------
//
// C++ course assignment code 
//
// G. Papaioannou, 2013
//
//
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Image.h"
#include <algorithm>
namespace imaging 
{

//-------------------------------- Image class implementation goes here ---------------------------------------------------
	
		
		 Component * Image::getRawDataPtr() const{
			return buffer;
		 }
	
		Color Image::getPixel(unsigned int x, unsigned int y) const{
			Color col;
			if(x>width || y>height){ //Ektos oriwn ? Epestrepse ena Color me (0,0,0) Comp
				col.x=0;
				col.y=0;
				col.z=0;
			}
			else if(interleaved){//Einai PPM?
				col.x=buffer[(x-1)*(3*width)+3*(y-1)];//Edw brisketai to Red 
				col.y=buffer[(x-1)*(3*width)+3*(y-1)+1];//Pame apla mia 8esi pio pera
				col.z=buffer[(x-1)*(3*width)+3*(y-1)+2];//Pame allh mia 
			}
			else{//Afou dn einai PPM einai CPI
				col.x=buffer[(x-1)*width+(y-1)];//Edw brisketai to Red
				col.y=buffer[(x-1)*width+(y-1)+width*height];//Proxwra me bhma width*height
				col.z=buffer[(x-1)*width+(y-1)+width*height+width*height];//Proxwra pali me width*height
			}
			return col;

			}
		void Image::setPixel(unsigned int x,unsigned int y,Color value){
			if(x>width || y>height){//Einai kapoia suntetagmenh out of bounds? 
				return;
			}
			else if(interleaved){//Einai PPM?
				buffer[(x-1)*(3*width)+3*(y-1)]=value.x; //Paromoia me thn get alla edw allazoume  times 
				buffer[(x-1)*(3*width)+3*(y-1)+1]=value.y;
				buffer[(x-1)*(3*width)+3*(y-1)+2]=value.z;
			}
			else{//Afou dn einai PPM einai CPI
				buffer[(x-1)*width+(y-1)]=value.x; //Paromoia me thn get alla edw allazoume mia times
				buffer[(x-1)*width+(y-1)+width*height]=value.y;
				buffer[(x-1)*width+(y-1)+width*height+width*height]=value.z;
			}
		}
		void Image::setData(const Component * & data_ptr){
			memcpy(buffer,* & data_ptr,3*width*height*sizeof(Component));//Xrhsh ths memcpy gia na ginei deep-copy tou pinaka
		}
		void Image::convertToInterleavedImage(){
			if(interleaved){//An einai PPM return
				return;
			}
			int j=0;
			Component* temp=new Component[3*width*height]; //Enas prosorinos pinakas 
			for(unsigned int i=0;i<3*width*height;i=i+3){
				temp[i]=buffer[j];//To Red brisketai edw
				temp[i+1]=buffer[j+width*height];//To Green brisketai apla meta apo width*height 8eseis
				temp[i+2]=buffer[j+2*width*height];//To black brisketai meta apo 2*width*height 8eseis 
				j++;//To j ousiastika mas leei poses 3ades exoume grapsei mexri twra gia na 3eroume pou na koitazoume ston buffer
			}
			memcpy(buffer,temp,3*width*height*sizeof(Component));//Antegrapse ton temp ston buffer
			interleaved=true;//Alla3e to flag
			delete[] temp;//Kane delete thn temp
		}
		void Image::convertToColorPlaneImage(){
			if(!interleaved){ //An einai CPI return
				return;
			}
				int j=0;
				Component* temp=new Component[3*width*height]; 
				for(unsigned int i=0;i<3*width*height;i=i+3){ //Apla kanei thn antistrofh diadikasia p kaname sthn converTtoInterLeaved
				temp[j]=buffer[i];
				temp[j+width*height]=buffer[i+1];
				temp[j+2*width*height]=buffer[i+2];
				j++;
				}
				memcpy(buffer,temp,3*width*height*sizeof(Component)); //Antegrapse to prosorino ston buffer
				interleaved=false; //Alla3e to flag
				delete[] temp; //Apodesmeuse mnhmh
				
		}
		Image::Image(unsigned int width, unsigned int height, bool interleaved): //Apla dinoume times sta pedia kai desmeuoume ena pinaka me to katallhlo megethos
			width(width),height(height),interleaved(interleaved){
			buffer=new Component[width*height*3];
		}
		Image::Image(unsigned int width, unsigned int height, const Component * data_ptr, bool interleaved):
			width(width),height(height),interleaved(interleaved){
			buffer=new Component[width*height*3];
			setData(data_ptr); //Setaroume kai ton pinaka xrhsimopoiwntas ton pointer data_ptr
		}
		Image::Image(const Image &src):
			width(src.getWidth()),// edw apla kanoume get tis times tou src gia na arxikopoihsoume ta pedia
			height(src.getHeight()),
			interleaved(src.isInterleaved()){
			buffer=new Component[width*height*3];
			Component const *tempbuff=src.getRawDataPtr(); //apo8hkeuoume ena deikth p deixnei ston pinaka tou src
			setData(tempbuff);//Setareton
		}
		Image::~Image(){ //destructor
			delete[] buffer;
		}

	
	
//-------------------------------- Helper functions -----------------------------------------------------------------------

	bool isSuffix(std::string & filename, const std::string suffix)
	{
		std::string low_caps_filename = filename;
		std::string dot_suffix = '.' + suffix;
		
		// make the string lowercase.
		std::transform(low_caps_filename.begin(), low_caps_filename.end(), low_caps_filename.begin(), ::tolower );

		//make the suffix lowercase.
		std::transform(dot_suffix.begin(), dot_suffix.end(), dot_suffix.begin(), ::tolower );
		
		size_t pos = low_caps_filename.rfind(dot_suffix);
		
		if (pos == std::string::npos) // did not find it
			return false;

		if (pos < low_caps_filename.length()-dot_suffix.length()) // found it, but not at the end
			return false;
		
		return true; // now we are positive. :)
	}

	void replaceSuffix(std::string & filename, const std::string suffix)
	{
		size_t pos = filename.rfind('.');
		if (pos==std::string::npos)
			return;

		filename.resize(pos+1);
		filename+=suffix;
	}


} // namespace imaging