#include "zupply.hpp"
using namespace zz;

int main()
{
	Image img("graf.jpg");
	Image img2 = img;
	img.resize(0.5);
	img.save("graf_half.jpg", 80);
	img2.crop(Point(10, 10), Point(50, 50));
	img2.save("graf_crop.png");
}