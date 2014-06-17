#include <iostream>
#include <string>
#include "src/obj.h"

using namespace std;

int main(){
	//~ std::string nameFile = "models/MultiTextures.obj";
	std::string nameFile = "models/rossini.obj";

	cObj myObj(nameFile);
	
	return 0;
}
