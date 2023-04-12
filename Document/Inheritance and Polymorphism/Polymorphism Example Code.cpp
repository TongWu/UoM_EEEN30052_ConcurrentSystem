// Polymorphism example based on 2D shapes

#include <iostream>
#include <cmath>
#include <string>

using namespace std;

const double PI = 3.1415926;

class Shape {                                                   // abstract base class
private:
    double area;                                                // all subclasses have area and perimeter data members
    double perimeter;
public:
    Shape() {}
    virtual double calcArea() = 0;                              // = 0 means implementation is deferred to subclass, so Shape cannot be instantiated
    virtual double calcPerimeter() = 0;
    virtual string getKind() = 0;
};

class Rectangle : public Shape {                                // concrete (instantiatable) class that inherits from Shape
private:
    double base, height;
public:
    Rectangle(double b, double ht) : base(b), height(ht) { }
    double calcArea() { return base * height; }                 // function can be created because the formula for the area of a rectangle is known
    double calcPerimeter() { return 2.0 * base * height; }      // function can be created because the formula for the perimeter of a rectangle is known
    string getKind (){ return "Rectangle"; }                    // function can be created because kind of shape is known
};

class Triangle : public Shape {
private:
    double side1, side2, side3; // length of another side of triangle besides base
public:
    Triangle(double s1, double s2, double s3) : side1(s1), side2(s2), side3(s3) {}
    double calcArea() {                                         // function can be created because the formula for the area of a triangle is known
        double p = calcPerimeter();                             // function can be created because the formula for the perimeter of a triangle is known
        return sqrt(p*(p-side1) * (p - side2) * (p - side3));   // function can be created because kind of shape is known
    }
    double calcPerimeter() {
        return side1 + side2 + side3;
    }
    string getKind() { return "Triangle"; }
};


class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double calcArea() { return PI * radius * radius; }          // function can be created because the formula for the area of a circle is known
    double calcPerimeter() { return 2.0 * PI * radius; }        // function can be created because the formula for the perimeter of a citrcle is known
    string getKind() { return "Circle"; }                       // function can be created because kind of shape is known
};

void printShape(Shape& s) {                                     // will work with objects of ANY subclass of Shape
    cout << s.getKind() << " area = " << s.calcArea() << " perimeter = " << s.calcPerimeter() << endl;
}

int main(){
    Circle c1(2.0);
    Rectangle r1(2.0, 4.0);
    Triangle t1(10.0, 5.0, 4.0);
    std::cout << "Hello World!\n";
    printShape(c1);
    printShape(r1);
    printShape(t1);
}