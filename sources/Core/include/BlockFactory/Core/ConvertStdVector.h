/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_CONVERTSTDVECTOR_H
#define WBT_CONVERTSTDVECTOR_H

#include <string>
#include <vector>

// Details about templates in http://drake.mit.edu/cxx_inl.html

// Template declaration
// ====================

// Its definition will be in the cpp file. This is allowed because all the allowed variant are
// either explicitly declared or explicitly specialized.
namespace wbt {
    template <typename Tin, typename Tout>
    void convertStdVector(const std::vector<Tin>& input, std::vector<Tout>& output);
}

// Explicit declaration for all the other supported types
// ========================================================

// Int to other numeric types
extern template void wbt::convertStdVector<int, int>(const std::vector<int>& input,
                                                     std::vector<int>& output);
extern template void wbt::convertStdVector<int, bool>(const std::vector<int>& input,
                                                      std::vector<bool>& output);
extern template void wbt::convertStdVector<int, double>(const std::vector<int>& input,
                                                        std::vector<double>& output);
// Bool to other numeric types
extern template void wbt::convertStdVector<bool, bool>(const std::vector<bool>& input,
                                                       std::vector<bool>& output);
extern template void wbt::convertStdVector<bool, int>(const std::vector<bool>& input,
                                                      std::vector<int>& output);
extern template void wbt::convertStdVector<bool, double>(const std::vector<bool>& input,
                                                         std::vector<double>& output);
// Double to other numeric types
extern template void wbt::convertStdVector<double, double>(const std::vector<double>& input,
                                                           std::vector<double>& output);
extern template void wbt::convertStdVector<double, int>(const std::vector<double>& input,
                                                        std::vector<int>& output);
extern template void wbt::convertStdVector<double, bool>(const std::vector<double>& input,
                                                         std::vector<bool>& output);
// String to string
extern template void
wbt::convertStdVector<std::string, std::string>(const std::vector<std::string>& input,
                                                std::vector<std::string>& output);

// Explicit specialization for std::string type
// ============================================

namespace wbt {
    // String to numeric
    template <>
    void convertStdVector<std::string, int>(const std::vector<std::string>& input,
                                            std::vector<int>& output);
    template <>
    void convertStdVector<std::string, bool>(const std::vector<std::string>& input,
                                             std::vector<bool>& output);
    template <>
    void convertStdVector<std::string, double>(const std::vector<std::string>& input,
                                               std::vector<double>& output);
    // Numeric to string
    template <>
    void convertStdVector<int, std::string>(const std::vector<int>& input,
                                            std::vector<std::string>& output);
    template <>
    void convertStdVector<bool, std::string>(const std::vector<bool>& input,
                                             std::vector<std::string>& output);
    template <>
    void convertStdVector<double, std::string>(const std::vector<double>& input,
                                               std::vector<std::string>& output);
} // namespace wbt

#endif
