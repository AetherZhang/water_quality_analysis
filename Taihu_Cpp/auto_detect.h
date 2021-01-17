//
//  auto_detect.h
//  Taihu_C
//
//  Created by AetherZh on 2020/9/5.
//  Copyright © 2020 AetherZh. All rights reserved.
//

#ifndef AUTO_DETECT_H
#define AUTO_DETECT_H

#include <map>
#include <vector>

#include <opencv2/opencv.hpp>


const int CHANNEL = 7;

struct coefficient_t {
    double k;
    double re;
};

using dict_t = std::map<int, std::vector<double>>;

// Read the dict csv file.
dict_t dict_read(const std::string& file_path);
// To get the water area.
std::vector<cv::Mat_<double>> get_water_area(const std::vector<cv::Mat>& imgs,
                                             const std::vector<double>& reflectance,
                                             const dict_t& rad_dict,
                                             int cur_rad,
                                             double thresh);
// Chla model.
cv::Mat_<double> chla(const std::vector<cv::Mat_<double>>& water_area, coefficient_t coefficient);
// TP model.
cv::Mat_<double> tp(const std::vector<cv::Mat_<double>>& water_area, coefficient_t coefficient);
// Save the mat data into a csv file.
void save_as_csv(const std::string& file_name, const cv::Mat_<double>& data);
// Find the closest numb in vector.
int find_closest(const std::vector<int>& vec, int val);

#endif /* AUTO_DETECT_H */
