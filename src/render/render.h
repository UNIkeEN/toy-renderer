#pragma once

#include "utils/enum.h"

class Render {
public:
    virtual ~Render() = default;

    virtual void init() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    virtual RENDERER_TYPE getType() const = 0;
};
