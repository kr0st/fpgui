#ifndef HSV_RGB_CONVERTER_H
#define HSV_RGB_CONVERTER_H


class Hsv_Rgb_Converter
{
    public:

        typedef struct
        {
            double r;       // a fraction between 0 and 1
            double g;       // a fraction between 0 and 1
            double b;       // a fraction between 0 and 1
        } rgb;

        typedef struct
        {
            double h;       // angle in degrees
            double s;       // a fraction between 0 and 1
            double v;       // a fraction between 0 and 1
        } hsv;

        static hsv rgb2hsv(rgb in);
        static rgb hsv2rgb(hsv in);
};

#endif // HSV_RGB_CONVERTER_H
