//
//  auto_detect.cpp
//  Taihu_Cpp
//
//  Created by AetherZh on 2020/9/6.
//  Copyright © 2020 AetherZh. All rights reserved.
//

#include <stdio.h>

#include "auto_detect.h"

dict_t dict_read(const std::string& file_path)
{
    dict_t dict;
    FILE* in = nullptr;
    if ((in = fopen(file_path.c_str(), "r")) == nullptr)
    {
        printf("The file %s can not be opened.\n", file_path.c_str());
        return dict;
    }
    
    int key;
    std::vector<double> DN {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    while (fscanf(in, "%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf",
                  &key, &DN[0], &DN[1], &DN[2], &DN[3], &DN[4], &DN[5], &DN[6]) != EOF)
    {
        dict[key] = DN;
    }
    
    fclose(in);
    return dict;
}

std::vector<cv::Mat_<double>> get_water_area(const std::vector<cv::Mat>& imgs,
                                             const std::vector<double>& reflectance,
                                             const dict_t& rad_dict,
                                             int cur_rad,
                                             double thresh)
{
    std::vector<cv::Mat_<double>> water_area;
    
    std::vector<int> rad;
    for (auto& i : rad_dict)
    {
        rad.emplace_back(i.first);
    }
    int key = find_closest(rad, cur_rad);
    std::vector<double> flec_DN = rad_dict.at(key);
    for (int i = 0; i < CHANNEL; ++i)
    {
        water_area.emplace_back(reflectance[i] / flec_DN[i] * cv::Mat_<double>(imgs[i]));
    }
    
    cv::Mat_<double> ndwi;
    divide(water_area[6] - water_area[2], water_area[6] + water_area[2], ndwi);
    
    for (auto& i : ndwi)
    {
        i = i <= thresh ? 1 : NAN;
    }
    
    for (int i = 0; i < CHANNEL; ++i)
    {
        water_area[i] = water_area[i].mul(ndwi);
    }
    
    return water_area;
}

cv::Mat_<double> chla(const std::vector<cv::Mat_<double>>& water_area, coefficient_t coefficient)
{
    cv::Mat_<double> part1, part2;
    divide(1, water_area[4] + 0.0001f, part1);
    divide(1, water_area[5] + 0.0001f, part2);
    cv::Mat_<double> rst = coefficient.k * (part1 - part2).mul(water_area[6]) + coefficient.re;
    
//    for (auto& i : rst)
//    {
//        i = i < 0 ? 0 : i;
//    }
    return rst;
}

cv::Mat_<double> tp(const std::vector<cv::Mat_<double>>& water_area, coefficient_t coefficient)
{
    cv::Mat_<double> part;
    divide(water_area[6], water_area[2], part);
    cv::Mat_<double> rst = coefficient.k * part + coefficient.re;
    
//    for (auto& i : rst)
//    {
//        i = i < 0 ? 0 : i;
//    }
    return rst;
}

void save_as_csv(const std::string& file_name, const cv::Mat_<double>& data)
{
    FILE* out = nullptr;
    if ((out = fopen(file_name.c_str(), "w")) == nullptr)
    {
        printf("The file %s can not be opened.\n", file_name.c_str());
        return;
    }
    int col;
    for (col = 0; col < data.cols - 1; ++col)
    {
        fprintf(out, "%d,", col);
    }
    fprintf(out, "%d\n", col);
    
    int col_cnt = 0;
    char split;
    for (auto& i : data)
    {
        if (!isnan(i))
        {
            fprintf(out, "%.13lf", i);
        }
        
        ++col_cnt;
        if (col_cnt < data.cols)
        {
            split = ',';
        }else
        {
            col_cnt = 0;
            split = '\n';
        }
        fprintf(out, "%c", split);
    }
    
    fclose(out);
}

int find_closest(const std::vector<int>& vec, int val)
{
    assert(vec.size() > 0);
    
    int closest = vec[0];
    int dt = abs(val - closest), t;
    for (int i = 1; i < vec.size(); ++i)
    {
        if ((t = abs(val - vec[i])) < dt)
        {
            dt = t;
            closest = vec[i];
        }
    }
    
    return closest;
}

