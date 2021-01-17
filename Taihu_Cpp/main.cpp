#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <opencv2/opencv.hpp>

#include "auto_detect.h"

const std::string test_path[CHANNEL + 3] {
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00443.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00490.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00560.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00620.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00665.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00709.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/TaiHuautodect/SampleImages/20200904/01_20200904080009_00865.bmp",
    "/Users/aether/Codes/Auto_Dect/TaiHuautodect/randiance/radiance.csv",
    "Chla_rst.csv",
    "Tp_rst.csv"
};

const int channel[CHANNEL] {443, 490, 560, 620, 665, 709, 865};
const std::vector<double> standard_reflectance {0.275f, 0.273f, 0.283f, 0.295f, 0.297f, 0.292f, 0.282f};
std::vector<cv::Mat> imgs;
dict_t dict;

int main(int argc, char* argv[])
{
    // To read different channel imgs and src img.
    cv::Mat img;
    cv::String img_path;
    for (int i = 0; i < CHANNEL; ++i)
    {
        std::cout << "input channel[" << channel[i] << "] image path:";
//        std::cin >> img_path;
        img_path = test_path[i];
        img = cv::imread(img_path, cv::IMREAD_UNCHANGED);
        if (img.data == nullptr)
        {
            puts("Img no data! Exiting the program");
            return -1;
        }
        imgs.emplace_back(img);
    }
    
    // To read dict.
    std::string dict_path;
    puts("input dict file path:");
//    std::cin >> dict_path;
    dict_path = test_path[CHANNEL + 0];
    dict = dict_read(dict_path);
    if (dict.empty())
    {
        puts("Dict no data! Exiting the program");
        return -1;
    }
    
    double thresh = -0.1f;
    int cur_rad = 38205;
//    puts("input [(double)thresh] & [(int)current rad]:");
//    std::cin >> thresh >> cur_rad;
    
    // To calculate the water area.
    std::vector<cv::Mat_<double>> water_area = get_water_area(imgs, standard_reflectance, dict, cur_rad, thresh);
    
    coefficient_t chla_coef, tp_coef;
    
    chla_coef.k = 242.05f;
    chla_coef.re = 123.11f;
//    puts("input Chla model [(double)k] & [(double)re]:");
//    std::cin >> chla_coef.k >> chla_coef.re;
    cv::Mat_<double> chla_rst = chla(water_area, chla_coef);

    tp_coef.k = 0.03238f;
    tp_coef.re = 0.0985f;
//    puts("input TP model [(double)k] & [(double)re]:");
//    std::cin >> tp_coef.k >> tp_coef.re;
    cv::Mat_<double> tp_rst = tp(water_area, tp_coef);
    
    std::string rst_name;
    
    puts("input Chla model result name:");
//    std::cin >> rst_name;
    rst_name = test_path[CHANNEL + 1];
    save_as_csv(rst_name, chla_rst);
    std::cout << "Save " << rst_name << " suc!\n";
    
    puts("input Tp model result name:");
//    std::cin >> rst_name;
    rst_name = test_path[CHANNEL + 2];
    save_as_csv(rst_name, tp_rst);
    std::cout << "Save " << rst_name << " suc!\n";
    
    return 0;
}
