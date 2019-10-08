#include <string>
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/geometry/geometries/geometries.hpp>

#include <boost/foreach.hpp>


bool shareSide(const boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > &box1, 
	const boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > &box2){

		if(box1.max_corner().x() == box2.min_corner().x() 
			&& box1.max_corner().y() == box2.max_corner().y()){
			return true;
		}
		if(box1.max_corner().y() == box2.min_corner().y() 
			&& box1.max_corner().x() == box2.max_corner().x()){
			return true;
		}
		if(box2.max_corner().x() == box1.min_corner().x() 
			&& box2.max_corner().y() == box1.max_corner().y()){
			return true;
		}
		if(box2.max_corner().y() == box1.min_corner().y() 
			&& box2.max_corner().x() == box1.max_corner().x()){
			return true;
		}
		return false;
}

void doMergeBoxes(std::vector<boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > > & dest){
		// if any can be merged, merge and call back into this
		for(int i = 0; i < dest.size(); i++){			
			for(int j = i+1; j < dest.size(); j++){
				//boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > rect1 = dest[i];
				//boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > rect2 = dest[j];
				if(shareSide(dest[i], dest[j])){
					std::cout << "merging" << std::endl;
					//dest.erase(dest.begin()+i);
					//boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > expanded = 
					//	boost::geometry::make_inverse<boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > >();
					boost::geometry::expand(dest[i], dest[j]);
					dest.erase(dest.begin()+j);
					//Rectangle merged = mergeRects(rect1, rect2);
					//System.out.println("...result is: " + merged);
					//vectRects.insertElementAt(merged, i);
					//vectRects = 
					doMergeBoxes(dest);
				}
			}
		}
}

void mergeBoxes(
	std::vector<boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > > & dest,
	std::vector<boost::geometry::model::box<boost::geometry::model::d2::point_xy<int> > > const & boxes){
	
	// copy source boxes into dest
	for(int i = 0; i < boxes.size(); i++){
		dest.push_back(boxes[i]);
	}
	doMergeBoxes(dest);
}

int main(int argc, char* argv[])
{
	//using namespace boost::geometry;

	using boost::geometry::expand;

	typedef boost::geometry::model::d2::point_xy<int> point;
	typedef boost::geometry::model::box<point> box;

	point p_0_0(0, 0), p_256_256(256, 256), p_256_0(256, 0), p_512_256(512, 256), p_0_256(0, 256), p_256_512(256, 512),
		p_0_512(0, 512), p_256_768(256, 768), p_512_1024(512, 1024);
	//std::cout << "Distance p1-p2 is: " << distance(p1, p2) << std::endl;

	box top_left(p_0_0, p_256_256), top_middle(p_256_0, p_512_256), left_middle(p_0_256, p_256_512),
		left_bottom(p_0_512, p_256_768), alone(p_256_768, p_512_1024);

	std::cout << boost::geometry::area(top_left) << std::endl;
	std::cout << boost::geometry::area(top_middle) << std::endl;
	std::cout << boost::geometry::area(left_middle) << std::endl;    

	std::string str("yo yo yo");

	std::cout << "share side (top_left and top_middle)" << shareSide(top_left, top_middle) << std::endl;
	std::cout << "share side (top_middle and top_left)" << shareSide(top_middle, top_left) << std::endl;
	std::cout << "share side (top_left and left_middle)" << shareSide(top_left, left_middle) << std::endl;
	std::cout << "share side (left_middle and top_left)" << shareSide(left_middle, top_left) << std::endl;
	std::cout << "share side (top_middle and left_middle)" << shareSide(top_middle, left_middle) << std::endl;
	std::cout << "share side (left_middle and top_middle)" << shareSide(left_middle, top_middle) << std::endl;
	std::cout << "share side (top_left and left_bottom)" << shareSide(top_left, left_bottom) << std::endl;
	std::cout << "share side (left_bottom and top_left)" << shareSide(left_bottom, top_left) << std::endl;
	std::cout << "share side (left_middle and left_bottom)" << shareSide(left_middle, left_bottom) << std::endl;
	std::cout << "share side (left_bottom and left_middle)" << shareSide(left_bottom, left_middle) << std::endl;
	std::cout << "share side (left_bottom and alone)" << shareSide(left_bottom, alone) << std::endl;
	std::cout << "share side (alone and left_bottom)" << shareSide(alone, left_bottom) << std::endl;

	box expanded = boost::geometry::make_inverse<box>();

	expand(expanded, top_left);
	expand(expanded, top_middle);

	std::cout << boost::geometry::dsv(expanded) << std::endl;

	std::vector<box> mergedBoxes;
	std::vector<box> originalBoxes;
	
	//for(int i = 0; i < originalBoxes.size(); i++){
	originalBoxes.push_back(top_left);
	originalBoxes.push_back(top_middle);
	originalBoxes.push_back(left_middle);
	originalBoxes.push_back(left_bottom);
	originalBoxes.push_back(alone);

	mergeBoxes(mergedBoxes, originalBoxes);

	std::cout << mergedBoxes.size() << std::endl;	

	std::cout << "original:" << std::endl;
	for(int i = 0; i < originalBoxes.size(); i++){
		std::cout << boost::geometry::dsv(originalBoxes[i]) <<  " area:" << boost::geometry::area(originalBoxes[i]) << std::endl;
	}

	std::cout << "merged:" << std::endl;
	for(int i = 0; i < mergedBoxes.size(); i++){
		std::cout << boost::geometry::dsv(mergedBoxes[i]) << " area:" << boost::geometry::area(mergedBoxes[i]) << std::endl;
	}

	std::cin >> str;
	return 0;
}
