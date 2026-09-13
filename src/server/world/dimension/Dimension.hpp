#pragma once
#include <string>
#include "DimensionId.hpp"

class Dimension {
public:
    Dimension(DimensionId id, std::string name)
        : mName(std::move(name)), mId(id) {}

    DimensionId getId() const { return mId; }

private:
    std::string mName;
    DimensionId mId;
};