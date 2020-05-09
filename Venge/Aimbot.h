#pragma once

#include "../Venge/SDK/UserCmd.h"

namespace aimhelper {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
    void run(UserCmd*) noexcept;
}