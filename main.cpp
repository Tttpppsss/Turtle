#include "Turtle.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

int main(){
    int width, height;
    MyBmp img(width, height);
    float x, y;
    float length, angle;
    char side;
    unsigned char r, g, b;
    std::cin >> "Ширина, высота изображения (файла):" >> width, height;
    std::cin >> "Начальные координаты черепашки:" >> x, y;
    Drower Turtle(width, height, x, y);
    while (x <= height && y <= width){
        Turtle.q_up();
        std::cout << "Направление ('r' или 'l'):"; std::cin >> side;
        std::cout << "Длина шага:"; std::cin >> length;
        std::cout << "Угол:"; std::cin >> angle;
        std::cout << "Цвет(показател по r(red), далее g(green), потом b(blue)):"; std::cin >> r, g, b;
        if(side = 'r'){
            Turtle.right(angle); 
            Turtle.forward(length);
        }
        if(side = 'l'){
            Turtle.left(angle); 
            Turtle.forward(length);
        }
    }
    img.save("Res.bmp");
    return 0;
}