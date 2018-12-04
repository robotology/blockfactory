/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_LOG_H
#define WBT_LOG_H

#include <memory>
#include <sstream>
#include <string>

#ifdef NDEBUG
#define WBT_LOG_VERBOSITY wbt::Log::Verbosity::RELEASE
#else
#define WBT_LOG_VERBOSITY wbt::Log::Verbosity::DEBUG
#endif

#ifndef wbtError
#define wbtError                                 \
    wbt::Log::getSingleton().getLogStringStream( \
        wbt::Log::Type::ERROR, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef wbtWarning
#define wbtWarning                               \
    wbt::Log::getSingleton().getLogStringStream( \
        wbt::Log::Type::WARNING, __FILE__, __LINE__, __FUNCTION__)
#endif

namespace wbt {
    class Log;
} // namespace wbt

/**
 * @brief Class for handling log messages
 *
 * Errors and Warnings are currently supported.
 */
class wbt::Log
{
public:
    enum class Type
    {
        ERROR,
        WARNING
    };

    enum class Verbosity
    {
        RELEASE,
        DEBUG
    };

private:
    class impl;
    std::unique_ptr<impl> pImpl;

public:
    Log();
    ~Log() = default;

    /**
     * @brief Get the Log singleton
     *
     * There is only one instance in the whole program of this class.
     *
     * @return The log singleton.
     */
    static wbt::Log& getSingleton();

    /**
     * @brief Get the stringstream object for adding log messages
     *
     * @param type The log type.
     * @param file The file from which this method is called (preprocessor directive).
     * @param line The line from which this method is called (preprocessor directive).
     * @param function The function from which this method is called (preprocessor directive).
     * @return The stringstream object matching the log type.
     */
    std::stringstream& getLogStringStream(const Type& type,
                                          const std::string& file,
                                          const unsigned& line,
                                          const std::string& function);

    /**
     * @brief Get the stored error messages.
     * @return The error messages.
     */
    std::string getErrors() const;

    /**
     * @brief Get the stored warning messages.
     * @return The warning messages.
     */
    std::string getWarnings() const;

    /**
     * @brief Clear the stored error messages.
     */
    void clearErrors();

    /**
     * @brief Clear the stored warning messages.
     */
    void clearWarnings();

    /**
     * @brief Clear all the stored log messages.
     */
    void clear();
};

#endif // WBT_LOG_H
