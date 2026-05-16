#include "filters/FilterManager.h"
#include <stdexcept>

FilterManager::FilterManager() {
}

FilterManager::~FilterManager() {
}

void FilterManager::registerFilter(std::unique_ptr<Filter> filter) {
    std::string name = filter->getName();
    filters[name] = std::move(filter);
}

Filter* FilterManager::getFilter(const std::string& name) {
    auto it = filters.find(name);
    if (it == filters.end()) {
        return nullptr;
    }
    return it->second.get();
}

std::vector<std::string> FilterManager::getFilterList() const {
    std::vector<std::string> list;
    for (const auto& pair : filters) {
        list.push_back(pair.first);
    }
    return list;
}

void FilterManager::applyFilter(const std::string& name, PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) {
    Filter* filter = getFilter(name);
    if (!filter) {
        throw std::runtime_error("Filter not found: " + name);
    }
    filter->apply(pixelBuffer, params);
}
