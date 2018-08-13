#ifndef HSV_RGB_CONVERTER_H
#define HSV_RGB_CONVERTER_H


class Hsv_Rgb_Converter
{
    public:

        struct rgb
        {
            double r;       // a fraction between 0 and 1
            double g;       // a fraction between 0 and 1
            double b;       // a fraction between 0 and 1

            rgb(int r, int g, int b)
            {
                this->r = r; this->b = b; this->g = g;
                this->r /= 255; this->b /= 255; this->g /= 255;
            }

            rgb()
            {
                r = 0;
                b = 0;
                g = 0;
            }
        };

        struct hsv
        {
            double h;       // angle in degrees
            double s;       // a fraction between 0 and 1
            double v;       // a fraction between 0 and 1
        };

        static hsv rgb2hsv(rgb in);
        static rgb hsv2rgb(hsv in);
};

#endif // HSV_RGB_CONVERTER_H
