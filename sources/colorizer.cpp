#include "colorizer.h"

Colorizer::Colorizer():
uninited_(true)
{
}

Colorizer::Colorizer(Hsv_Rgb_Converter::rgb seed):
first_use_(true),
uninited_(false)
{
    latest_color_ = Hsv_Rgb_Converter::rgb2hsv(seed);
}

Hsv_Rgb_Converter::rgb Colorizer::colorize(const std::string& str)
{
    if (uninited_)
    {
        Hsv_Rgb_Converter::rgb defcol;

        defcol.b = 1;
        defcol.g = 1;
        defcol.r = 1;

        return defcol;
    }

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

void Colorizer::set_seed(Hsv_Rgb_Converter::rgb seed)
{
    uninited_ = false;
    first_use_ = true;
    latest_color_ = Hsv_Rgb_Converter::rgb2hsv(seed);
}

