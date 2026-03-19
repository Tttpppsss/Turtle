#include <string>
#include <vector>
#include <cstdint>

class MyBmp{
private:
    // #pragma pack(push, 1) - отключает выравнивание структуры в памяти
    // Без этого компилятор может добавить пустые байты между полями для оптимизации доступа
    // Но BMP файл хранит поля плотно, без промежутков - поэтому нам нужно точно такое же расположение
    #pragma pack(push, 1)
    
    // Структура пикселя - порядок BGR, потому что BMP формат хранит компоненты цвета в таком порядке
    // Это исторически сложилось в Windows: не RGB, а именно BGR (Blue-Green-Red)
    // Если бы мы хранили как RGB, при чтении файла прямым копированием цвета бы перемешались
    struct Pixel {
        uint8_t b;  // Синяя компонента - идёт первой в BMP файле
        uint8_t g;  // Зелёная компонента - идёт второй
        uint8_t r;  // Красная компонента - идёт третьей
    };
    
    // Структура заголовка BMP файла (BITMAPFILEHEADER) - 14 байт
    // Все поля должны идти строго друг за другом, поэтому #pragma pack(push, 1) обязателен
    struct BMPFileHeader {
        uint16_t bfType;      // Сигнатура 'BM' (0x4D42) - 2 байта
        uint32_t bfSize;      // Полный размер файла в байтах - 4 байта
        uint16_t bfReserved1; // Зарезервировано, должно быть 0 - 2 байта
        uint16_t bfReserved2; // Зарезервировано, должно быть 0 - 2 байта
        uint32_t bfOffBits;   // Смещение от начала файла до данных пикселей - 4 байта
    };
    
    // Структура информационного заголовка BMP (BITMAPINFOHEADER) - 40 байт
    // Тоже требует плотной упаковки без выравнивания
    struct BMPInfoHeader {
        uint32_t biSize;          // Размер этой структуры (40 байт) - 4 байта
        int32_t  biWidth;         // Ширина изображения в пикселях - 4 байта
        int32_t  biHeight;        // Высота изображения в пикселях - 4 байта
                                  // Положительное = снизу вверх, отрицательное = сверху вниз
        uint16_t biPlanes;        // Количество цветовых плоскостей (должно быть 1) - 2 байта
        uint16_t biBitCount;      // Бит на пиксель (1,4,8,16,24,32) - 2 байта
        uint32_t biCompression;   // Тип сжатия (0 = BI_RGB без сжатия) - 4 байта
        uint32_t biSizeImage;     // Размер изображения в байтах (0 для BI_RGB) - 4 байта
        int32_t  biXPelsPerMeter; // Горизонтальное разрешение (пикселей на метр) - 4 байта
        int32_t  biYPelsPerMeter; // Вертикальное разрешение (пикселей на метр) - 4 байта
        uint32_t biClrUsed;       // Количество используемых цветов (0 = максимально для битности) - 4 байта
        uint32_t biClrImportant;  // Количество важных цветов (0 = все важны) - 4 байта
    };
    
    // #pragma pack(pop) - восстанавливаем предыдущее выравнивание
    // Это хороший тон: изменять настройки только на время объявления структур
    #pragma pack(pop)
    
    // Хранение заголовков
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    
public:
    MyBmp() = delete;
    MyBmp(int height, int width);
    MyBmp(std::string path);
    int load(std::string path);
    int save(std::string path);
    std::vector<std::vector<Pixel>> data;
};
class Drower{
    public:
        Drower(int width, int height, float xpos, float ypos);
        void forward(float length);
        void left(float deg);
        void right(float deg);
        void q_d();
        void q_up();
        void drawLine(MyBmp& img, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b);
    private:
};
