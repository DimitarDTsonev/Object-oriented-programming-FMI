#pragma once
#include <iostream>
#include <cmath>

class Rectangle;
class Triangle;
class Circle;

class Shape   // Abstract class - no instances of Shape are allowed!
{
protected:
	struct point
	{
		point() :x(0), y(0){}
		point(int x, int y) :x(x), y(y){}
		int x;
		int y;
		double getDist(const point& other) const
		{
			int dx = x - other.x;
			int dy = y - other.y;

			return std::sqrt(dx*dx + dy*dy);
		}
	};
	const point& getPointAtIndex(size_t index) const;
private:
	point* points;
	size_t pointsCount;

	void copyFrom(const Shape& other);
	void free();


public:
	Shape(size_t pointsCount);

	Shape(const Shape& other);
	Shape& operator=(const Shape& other);

	virtual ~Shape();

	void setPoint(size_t pointIndex, int x, int y);

	virtual double getArea() const = 0;             //pure virtual
	virtual double getPer()  const;                 // non-pure virtual
	virtual bool isPointIn(int x, int y) const = 0; //pure virtual

	virtual bool isIntersectedWith(Shape* shape) = 0;
	virtual bool isIntersectedWithRectange(Rectangle* shape) = 0;
	virtual bool isIntersectedWithTriangle(Triangle* shape) = 0;
	virtual bool isIntersectedWithCircle(Circle* shape) = 0;

	virtual Shape* clone() const = 0;

};