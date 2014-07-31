#include <iostream>

using namespace std;
int main(){

	// relative -0.518842, -0.536978, 0.68281
	// adjusted -0.570988, -0.444406, 02

	float xL = 0.432082, yL = -0.767056, zL = 5.65779; 
	float xT = 0.950924, yT = -0.230078, zT = 4.97498;

	//relative -1.85093, 0.735479, 0.56597
	//~ float xL = 0.111595, yL = 0.620016, zL = 3.48444; 
	//~ float xT = 1.96253, yT = -0.115463, zT = 2.91847;
	
	//~ //relative -0.16017, -1.66502, 0.61576
	//~ float xL = -1.89011, yL = -3.56482, zL = 3.52278; 
	//~ float xT = -1.72994, yT = -1.8998, zT = 2.90702;
	
	float ratioZTL = zT/zL;
	
	float relativeX = xL*ratioZTL - xT;
	float relativeY = yL*ratioZTL - yT;
	float relativeZ = zL*ratioZTL - zT;
	
	cout << "relative " << relativeX <<", "<< relativeY <<", "<< relativeZ << endl;
	
	
	return 0;
}
