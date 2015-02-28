#include <iostream>

#include "Stereosystem.h"


int main(int argc, char* argv[])
{

	Camera left(NULL);
	Camera right(NULL);

	Stereosystem stereo(left,right);

	std::cout<<"Test"<<std::endl;
	return 0;
}