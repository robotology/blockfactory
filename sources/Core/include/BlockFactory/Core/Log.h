/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_LOG_H
#define BLOCKFACTORY_CORE_LOG_H

#include <memory>
#include <sstream>
#include <string>

#ifdef NDEBUG
#define BF_LOG_VERBOSITY blockfactory::core::Log::Verbosity::RELEASE
#else
#define BF_LOG_VERBOSITY blockfactory::core::Log::Verbosity::DEBUG
#endif

#ifndef bfError
#define bfError                                                 \
    blockfactory::core::Log::getSingleton().getLogStringStream( \
        blockfactory::core::Log::Type::ERROR, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef bfWarning
#define bfWarning                                               \
    blockfactory::core::Log::getSingleton().getLogStringStream( \
        blockfactory::core::Log::Type::WARNING, __FILE__, __LINE__, __FUNCTION__)
#endif

namespace blockfactory {
    namespace core {
        class Log;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Class for handling log messages
 *
 * Errors and Warnings are currently supported.
 */
class blockfactory::core::Log
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class impl;
    std::unique_ptr<impl> pImpl;
#endif

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
    static blockfactory::core::Log& getSingleton();

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

#endif // BLOCKFACTORY_CORE_LOG_H
