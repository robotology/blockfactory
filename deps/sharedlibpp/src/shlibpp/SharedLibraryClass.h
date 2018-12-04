/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SHAREDLIBPP_SHAREDLIBRARYCLASS_H
#define SHAREDLIBPP_SHAREDLIBRARYCLASS_H

#include <shlibpp/SharedLibraryClassFactory.h>

namespace shlibpp {

/**
 * Container for an object created using a factory provided by a shared library.
 * Used to ensure the object is destroyed by a method also provided by the
 * shared library.  Mixing creation and destruction methods could be very bad.
 */
template <typename T>
class SharedLibraryClass
{
public:

    /**
     * Constructor for empty instance.
     */
    SharedLibraryClass();

    /**
     * Constructor for valid instance of a class from a shared library.
     *
     * @param factory the factory to use to construct (and eventually
     * destroy) the instance.
     */
    SharedLibraryClass(SharedLibraryClassFactory<T>& factory);

    /**
     * Destructor.
     */
    virtual ~SharedLibraryClass();

    /**
     * Construct an instance using the specified factory.  If an
     * instance has already been made, it is destroyed.
     *
     * @param factory the factory to use to construct (and eventually
     * destroy) the instance.
     * @return true on success
     */
    bool open(SharedLibraryClassFactory<T>& factory);

    /**
     * Destroy an instance if one has been created.
     *
     * @return true on success
     */
    virtual bool close();

    /**
     * Gives access to the created instance.
     *
     * No check made to ensure an instance is in fact present.
     * Call SharedLibraryClass::isValid first if unsure.
     *
     * @return the created instance
     */
    T& getContent();

    /**
     * Gives access to the created instance (const version).
     *
     * No check made to ensure an instance is in fact present.
     * Call SharedLibraryClass::isValid first if unsure.
     *
     * @return the created instance
     */
    const T& getContent() const;

    /**
     * Check whether a valid instance has been created.
     *
     * @return true iff a valid instance has been created
     */
    bool isValid() const;

    /**
     * Shorthand for SharedLibraryClass::getContent
     *
     * @return the created instance
     */
    T& operator*();

    /**
     * Shorthand for SharedLibraryClass::getContent (const version)
     *
     * @return the created instance
     */
    const T& operator*() const;

    /**
     * A pointer version of SharedLibraryClass::getContent
     *
     * @return a pointer to the created instance, or nullptr if there is none
     */
    T* operator->();

    /**
     * A pointer version of SharedLibraryClass::getContent (const version)
     *
     * @return a pointer to the created instance, or nullptr if there is none
     */
    const T* operator->() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    T* content;
    SharedLibraryClassFactory<T> *pfactory;
#endif
};

} // namespace shlibpp


#include <shlibpp/SharedLibraryClass-inl.h>

#endif // SHAREDLIBPP_SHAREDLIBRARYCLASS_H
