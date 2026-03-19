#include "Turtle.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

// Конструктор создания нового изображения
MyBmp::MyBmp(int height, int width) {
    // Инициализация заголовков для 24-битного BMP
    fileHeader.bfType = 0x4D42;  // 'BM'
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);  // 54 байта
    
    infoHeader.biSize = sizeof(BMPInfoHeader);  // 40 байт
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;  // положительное = снизу вверх
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;    // 24 бита на пиксель
    infoHeader.biCompression = 0;  // BI_RGB без сжатия
    infoHeader.biXPelsPerMeter = 2835;  // ~72 DPI
    infoHeader.biYPelsPerMeter = 2835;
    infoHeader.biClrUsed = 0;      // нет цветовой таблицы
    infoHeader.biClrImportant = 0;
    
    // Вычисляем размер строки с учётом выравнивания до 4 байт
    int rowSize = ((infoHeader.biWidth * infoHeader.biBitCount + 31) / 32) * 4;
    infoHeader.biSizeImage = rowSize * abs(infoHeader.biHeight);
    fileHeader.bfSize = fileHeader.bfOffBits + infoHeader.biSizeImage;
    
    // Создаём массив пикселей
    data.resize(abs(height));
    for (auto& row : data) {
        row.resize(width);
        // Заполняем чёрным цветом
        std::fill(row.begin(), row.end(), Pixel{0, 0, 0});
    }
}

// Конструктор загрузки из файла
MyBmp::MyBmp(std::string path) {
    if (load(path) != 0) {
        throw std::runtime_error("Failed to load BMP file: " + path);
    }
}

// Загрузка BMP файла
int MyBmp::load(std::string path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file " << path << std::endl;
        return -1;
    }
    
    // Читаем заголовки
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    
    // Проверяем сигнатуру
    if (fileHeader.bfType != 0x4D42) {
        std::cerr << "Error: Not a BMP file" << std::endl;
        return -2;
    }
    
    // Проверяем что это 24-битный BMP без сжатия
    if (infoHeader.biBitCount != 24) {
        std::cerr << "Error: Only 24-bit BMP supported" << std::endl;
        return -3;
    }
    
    if (infoHeader.biCompression != 0) {
        std::cerr << "Error: Only uncompressed BMP supported" << std::endl;
        return -4;
    }
    
    // Вычисляем параметры изображения
    int height = abs(infoHeader.biHeight);
    int width = infoHeader.biWidth;
    
    // Вычисляем выравнивание строк (BMP выравнивает каждую строку до кратного 4 байт)
    int rowSize = ((width * 24 + 31) / 32) * 4;
    int padding = rowSize - width * 3;
    
    // Перемещаемся к данным пикселей
    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    
    // Изменяем размер вектора данных
    data.resize(height);
    for (auto& row : data) {
        row.resize(width);
    }
    
    // Читаем пиксели построчно
    // В BMP строки хранятся снизу вверх если biHeight > 0
    if (infoHeader.biHeight > 0) {
        // Изображение снизу вверх - читаем с последней строки
        for (int i = height - 1; i >= 0; --i) {
            file.read(reinterpret_cast<char*>(data[i].data()), width * sizeof(Pixel));
            // Пропускаем выравнивание
            if (padding > 0) {
                file.seekg(padding, std::ios::cur);
            }
        }
    } else {
        // Изображение сверху вниз - читаем как обычно
        for (int i = 0; i < height; ++i) {
            file.read(reinterpret_cast<char*>(data[i].data()), width * sizeof(Pixel));
            if (padding > 0) {
                file.seekg(padding, std::ios::cur);
            }
        }
    }
    
    return 0;
}

// Сохранение BMP файла
int MyBmp::save(std::string path) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot create file " << path << std::endl;
        return -1;
    }
    
    int height = data.size();
    if (height == 0) return -2;
    int width = data[0].size();
    
    // Обновляем заголовки под текущие размеры
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    
    // Вычисляем выравнивание строк
    int rowSize = ((width * 24 + 31) / 32) * 4;
    int padding = rowSize - width * 3;
    
    infoHeader.biSizeImage = rowSize * height;
    fileHeader.bfSize = fileHeader.bfOffBits + infoHeader.biSizeImage;
    
    // Записываем заголовки
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    
    // Записываем пиксели
    // BMP ожидает строки снизу вверх если biHeight > 0
    char paddingBytes[4] = {0, 0, 0, 0};
    
    for (int i = height - 1; i >= 0; --i) {
        // Записываем строку пикселей
        file.write(reinterpret_cast<char*>(data[i].data()), width * sizeof(Pixel));
        // Добавляем выравнивание
        if (padding > 0) {
            file.write(paddingBytes, padding);
        }
    }
    
    return 0;
}

class Drower{
    public:
        Drower(int width, int height, float xpos, float ypos);
        const float pi = acos(-1.0);
        void forward(float length) {
            float dx = std::cos(angle * pi / 180);
            float dy = std::sin(angle * pi / 180);

            for(int i = 0; i < int(length); i++) {
                if(q_d && xpos >= 0 && ypos >= 0 && xpos < field[0].size() && ypos < field.size()){
                    field[int(ypos)][int(xpos)] = true;
                }
                xpos += dx;
                ypos += dy;
            }
        }

        void right(float deg) { angle += deg; }
        void left(float deg)  { angle -= deg; }
        void q_up()   { qd = false; }
        void q_d() { qd = true; }
        void drawLine(MyBmp& img, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b){
            int dx = abs(x2 - x1);
            int dy = abs(y2 - y1);

            int sx = (x1 < x2) ? 1 : -1;
            int sy = (x1 < x2) ? 1 : -1;

            int x = x1;
            int y = y1;

            if (dx > dy) {
                int error = 2*dy - dx;
        
                for (int i = 0; i <= dx; i++) {
                    img.data[x][y] = {r, g, b};
            
                    while (error >= 0) {
                        y += sy;
                        error -= 2*dx;
                    }
            
                    x += sx;
                    error += 2*dy;
                }
            } else {
                int error = 2*dx - dy;
        
                for (int i = 0; i <= dy; i++) {
                    img.data[x][y] = {r, g, b};
            
                    while (error >= 0) {
                        x += sx;
                        error -= 2*dy;
                    }
            
                    y += sy;
                    error += 2*dx;
                }
            }
        };

        

    private:
        unsigned char r, g, b;
        float xpos, ypos, angle; 
        bool qd;
        std::vector<std::vector<bool>>& field; 
    
};
