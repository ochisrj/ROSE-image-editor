#pragma once

#include "filters/Filter.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class FilterManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Filter>> filters;

public:
    FilterManager();
    ~FilterManager();

    void registerFilter(std::unique_ptr<Filter> filter);
    Filter* getFilter(const std::string& name);
    std::vector<std::string> getFilterList() const;
    void applyFilter(const std::string& name, PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params);
};
