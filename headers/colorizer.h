#ifndef COLORIZER_H
#define COLORIZER_H

#include <hsv_rgb_converter.h>

#include <string>
#include <map>

class Colorizer
{
    public:

        Colorizer();
        Colorizer(Hsv_Rgb_Converter::rgb seed);
        Hsv_Rgb_Converter::rgb colorize(const std::string& str);
        void set_seed(Hsv_Rgb_Converter::rgb seed);


    private:

        std::map<std::string, Hsv_Rgb_Converter::rgb> colormap_;
        Hsv_Rgb_Converter::hsv latest_color_;
        bool first_use_;
        bool uninited_;
};

#endif // COLORIZER_H
