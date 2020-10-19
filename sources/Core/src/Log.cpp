/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Log.h"

#include <cassert>
#include <vector>

using namespace blockfactory::core;

class Log::impl
{
public:
    std::vector<std::unique_ptr<std::stringstream>> errorsSStream;
    std::vector<std::unique_ptr<std::stringstream>> warningsSStream;

    const Verbosity verbosity = BF_LOG_VERBOSITY;

    static std::string
    serializeVectorStringStream(const std::vector<std::unique_ptr<std::stringstream>>& ss);
};

Log::Log()
    : pImpl(std::make_unique<Log::impl>())
{}

Log& Log::getSingleton()
{
    static Log logInstance;
    return logInstance;
}

std::stringstream& Log::getLogStringStream(const Log::Type& type,
                                           const std::string& file,
                                           const unsigned& line,
                                           const std::string& function)
{
    switch (pImpl->verbosity) {
        case Log::Verbosity::RELEASE:
            switch (type) {
                case Log::Type::LOG_TYPE_ERROR:
                    pImpl->errorsSStream.emplace_back(new std::stringstream);
                    return *pImpl->errorsSStream.back();
                case Log::Type::LOG_TYPE_WARNING:
                    pImpl->warningsSStream.emplace_back(new std::stringstream);
                    return *pImpl->warningsSStream.back();
            }
            break;
        case Log::Verbosity::DEBUG:
            switch (type) {
                case Log::Type::LOG_TYPE_ERROR: {
                    pImpl->errorsSStream.emplace_back(new std::stringstream);
                    auto& ss = *pImpl->errorsSStream.back();
                    ss << std::endl
                       << file << "@" << function << ":" << std::to_string(line) << std::endl;
                    return ss;
                }
                case Log::Type::LOG_TYPE_WARNING: {
                    pImpl->warningsSStream.emplace_back(new std::stringstream);
                    auto& ss = *pImpl->warningsSStream.back();
                    ss << std::endl
                       << file << "@" << function << ":" << std::to_string(line) << std::endl;
                    return ss;
                }
            }
            break;
    }

    // This should never happen. It is here to avoid compiler warnings.
    assert(false);
    std::stringstream* dummy = nullptr;
    return *dummy;
}

std::string
Log::impl::serializeVectorStringStream(const std::vector<std::unique_ptr<std::stringstream>>& ss)
{
    std::stringstream output;

    for (const auto& ss_elem : ss) {
        output << ss_elem->str() << std::endl;
    }

    return output.str();
}

std::string Log::getErrors() const
{
    return impl::serializeVectorStringStream(pImpl->errorsSStream);
}

std::string Log::getWarnings() const
{
    return impl::serializeVectorStringStream(pImpl->warningsSStream);
}

void Log::clearWarnings()
{
    pImpl->warningsSStream.clear();
}

void Log::clearErrors()
{
    pImpl->errorsSStream.clear();
}

void Log::clear()
{
    clearErrors();
    clearWarnings();
}
