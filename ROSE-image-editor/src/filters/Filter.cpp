#include "filters/Filter.h"

Filter::Filter(const std::string& filterName, FilterCategory cat)
    : name(filterName), category(cat) {
}

Filter::~Filter() {
}
