#include <iostream>
#include <opencv2/opencv.hpp>

//~ bool operator<(const Marker &M1,const Marker&M2)
//~ {
  //~ return M1.id<M2.id;
//~ }

//~ // returns a marker coded as bitMatrix
//~ cv::Mat getBitMatrix(){
	//~ cv::Mat bitMatrix = cv::Mat::zeros(5,5,CV_8UC1);
  //~ 
  //~ // default marker
  //~ bitMatrix.at<uchar>(0,0) = 1;
  //~ bitMatrix.at<uchar>(1,1) = 1;
  //~ bitMatrix.at<uchar>(1,2) = 1;
  //~ bitMatrix.at<uchar>(1,3) = 1;
  //~ bitMatrix.at<uchar>(2,0) = 1;
  //~ bitMatrix.at<uchar>(2,2) = 1;
  //~ bitMatrix.at<uchar>(2,3) = 1;
  //~ bitMatrix.at<uchar>(2,4) = 1;
  //~ bitMatrix.at<uchar>(3,0) = 1;
  //~ bitMatrix.at<uchar>(3,2) = 1;
  //~ bitMatrix.at<uchar>(3,3) = 1;
  //~ bitMatrix.at<uchar>(3,4) = 1;
  //~ bitMatrix.at<uchar>(4,0) = 1;
  //~ bitMatrix.at<uchar>(4,2) = 1;
  //~ bitMatrix.at<uchar>(4,3) = 1;
  //~ bitMatrix.at<uchar>(4,4) = 1;
  //~ return bitMatrix;
//~ }
//~ 
cv::Mat getBitMatrix(){
	cv::Mat bitMatrix = cv::Mat::zeros(5,5,CV_8UC1);
  
  // default marker
  bitMatrix.at<uchar>(0,0) = 1;
  bitMatrix.at<uchar>(0,2) = 1;
  bitMatrix.at<uchar>(0,3) = 1;
  bitMatrix.at<uchar>(0,4) = 1;
  bitMatrix.at<uchar>(1,0) = 1;
  bitMatrix.at<uchar>(1,2) = 1;
  bitMatrix.at<uchar>(1,3) = 1;
  bitMatrix.at<uchar>(1,4) = 1;
  bitMatrix.at<uchar>(2,0) = 1;
  bitMatrix.at<uchar>(2,2) = 1;
  bitMatrix.at<uchar>(2,3) = 1;
  bitMatrix.at<uchar>(2,4) = 1;
  bitMatrix.at<uchar>(3,0) = 1;
  bitMatrix.at<uchar>(3,2) = 1;
  bitMatrix.at<uchar>(3,3) = 1;
  bitMatrix.at<uchar>(3,4) = 1;
  bitMatrix.at<uchar>(4,0) = 1;
  bitMatrix.at<uchar>(4,2) = 1;
  bitMatrix.at<uchar>(4,3) = 1;
  bitMatrix.at<uchar>(4,4) = 1;
  return bitMatrix;
}

cv::Mat rotate(cv::Mat in)
{
  cv::Mat out;
  in.copyTo(out);
  for (int i=0;i<in.rows;i++)
  {
    for (int j=0;j<in.cols;j++)
    {
      out.at<uchar>(i,j)=in.at<uchar>(in.cols-j-1,i);
    }
  }
  return out;
}


int hammDistMarker(cv::Mat bits)
{
  int ids[4][5]=
  {
    {1,0,0,0,0},
    {1,0,1,1,1},
    {0,1,0,0,1},
    {0,1,1,1,0}
  };
  
  int dist=0;
  
  for (int y=0;y<5;y++)
  {
    int minSum=1e5; //hamming distance to each possible word
    std::cout << "y " << y << std::endl;
    for (int p=0;p<4;p++)
    {
		std::cout << "p " << p << std::endl;
      int sum=0;
      //now, count
      for (int x=0;x<5;x++)
      {
		  std::cout << "x " << x ;
        sum += bits.at<uchar>(y,x) == ids[p][x] ? 0 : 1;
        std::cout << " sum " << sum << std::endl;
      }
      
      if (minSum>sum)
        minSum=sum;
    }
    
    //do the and
    dist += minSum;
  }
  
  return dist;
}


int mat2id(const cv::Mat &bits)
{
  int val=0;
  for (int y=0;y<5;y++)
  {
    val<<=1;
    if ( bits.at<uchar>(y,1)) val|=1;
    val<<=1;
    if ( bits.at<uchar>(y,3)) val|=1;
  }
  return val;
}

int getMarkerId(cv::Mat bitMatrix, int &nRotations)
{ 
  //check all possible rotations
  cv::Mat rotations[4];
  int distances[4];
  
  rotations[0] = bitMatrix;  
  distances[0] = hammDistMarker(rotations[0]);
  
  std::pair<int,int> minDist(distances[0],0);
  
  for (int i=1; i<4; i++)
  {
    //get the hamming distance to the nearest possible word
    rotations[i] = rotate(rotations[i-1]);
    distances[i] = hammDistMarker(rotations[i]);
    
    if (distances[i] < minDist.first)
    {
      minDist.first  = distances[i];
      minDist.second = i;
    }
  }
  
  nRotations = minDist.second;
  if (minDist.first == 0)
  {
    return mat2id(rotations[minDist.second]);
  }
  
  return -1;
}

void testMarkerId(){
	int markerId = -1;
	int nRotations;
	cv::Mat bitMatrix = getBitMatrix();
	markerId = getMarkerId(bitMatrix,nRotations);
	std::cout << "markerId "<< markerId << std::endl;
	std::cout << "rotations "<< nRotations << std::endl;
}

void testHammerDistance(){
	cv::Mat bitMatrix = getBitMatrix();
	int hammDistance = hammDistMarker(bitMatrix);
	std::cout << "hamming distance "  << hammDistance << std::endl;
}


int main(){
	//~ testMarkerId();
	testHammerDistance();
	return 0;
}
