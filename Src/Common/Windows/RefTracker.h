//==============================================================================
// Copyright (c) 2014 - 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class maintains a reference count against a thread id
//==============================================================================

#ifndef _REF_TRACKER_H_
#define _REF_TRACKER_H_

#include <map>
#include <mutex>
#include <windows.h>

/// \addtogroup Common
// @{

//
/// This class maintains a reference count against a thread id
//
class RefTrackerCounter
{
    bool m_IsUsingExternalMutex;             ///< set to true if using an external mutex, false otherwise
    std::mutex* m_pmutex;                    ///< a mutex (can be a handle to an external mutex)
    std::map<DWORD, int> m_mapInsideWrapper; ///< holds a mapping of a thread id and reference count

public:
    /// Constructor
    RefTrackerCounter();

    /// Destructor
    ~RefTrackerCounter();

    /// Constructor
    /// \param[in] pM  an input mutex
    RefTrackerCounter(std::mutex* pM);

    /// Specify to use an external mutex
    /// \param[in] pM  the input external mutex
    void UseExternalMutex(std::mutex* pM);

    /// Increment the reference count
    void operator++(int);

    /// Decrement the reference count
    void operator--(int);

    /// Equality operator
    /// \param[in] v  a reference count
    bool operator==(DWORD v);

    /// Greater operator
    /// \param[in] v  an input reference count
    bool operator>(DWORD v);

    /// Get the reference count
    /// \return the reference count
    DWORD GetRef();

private:
    /// Disable copy constructor
    /// \param[in] rhs  the input object
    RefTrackerCounter(const RefTrackerCounter& rhs);

    /// Disable assignment operator
    /// \param[in] rhs  the input object
    /// \return a reference to the object
    RefTrackerCounter& operator=(const RefTrackerCounter& rhs);
};

/// This class handles reference tracking
class RefTracker
{
    /// pointer to an unsigned long that contains the number of references
    RefTrackerCounter* m_dwVal;

public:
    /// increments dwVal on creation
    /// \param[in,out] dwVal  the reference counter object
    RefTracker(RefTrackerCounter* dwVal);

    /// decrements dwVal on destruction
    ~RefTracker()
    {
        //
        // Protect against the app doing something stupid (like releasing a
        // resource more than once)
        if ((*m_dwVal) > 0)
        {
            (*m_dwVal)--;
        }
        else
        {
            //Log( logWARNING, "RefTracker destructor called with m_dwVal == 0\n" );
        }
    }
};

// @}

#endif //_REF_TRACKER_H_