#include "colorizer.h"

Colorizer::Colorizer(Hsv_Rgb_Converter::rgb seed):
first_use_(true)
{
    latest_color_ = Hsv_Rgb_Converter::rgb2hsv(seed);
}

Hsv_Rgb_Converter::rgb Colorizer::colorize(const std::string& str)
{
    auto iter = colormap_.find(str);
    if (iter != colormap_.end())
        return iter->second;

    if (first_use_)
        first_use_ = false;
    else
    {
        double golden_ratio_conjugate = 0.618033988749895;
        latest_color_.h = latest_color_.h * golden_ratio_conjugate;
    }

    auto rgb = Hsv_Rgb_Converter::hsv2rgb(latest_color_);
    colormap_[str] = rgb;

    return rgb;
}
