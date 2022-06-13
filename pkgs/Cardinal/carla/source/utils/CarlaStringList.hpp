/*
 * Carla String List
 * Copyright (C) 2014-2019 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#ifndef CARLA_STRING_LIST_HPP_INCLUDED
#define CARLA_STRING_LIST_HPP_INCLUDED

#include "LinkedList.hpp"

// -----------------------------------------------------------------------
// Helper class to manage the lifetime of a "char**" object

class CharStringListPtr
{
public:
    CharStringListPtr() noexcept
        : fCharList(nullptr) {}

    CharStringListPtr(const char* const* const c) noexcept
        : fCharList(c) {}

    CharStringListPtr(const CharStringListPtr& ptr) noexcept
        : fCharList(nullptr)
    {
        copy(ptr.fCharList);
    }

    CharStringListPtr(const LinkedList<const char*>& list) noexcept
        : fCharList(nullptr)
    {
        copy(list);
    }

    ~CharStringListPtr() noexcept
    {
        clear();
    }

    // -------------------------------------------------------------------

    operator const char* const*() const noexcept
    {
        return fCharList;
    }

    CharStringListPtr& operator=(const char* const* const c) noexcept
    {
        clear();
        fCharList = c;
        return *this;
    }

    CharStringListPtr& operator=(const CharStringListPtr& ptr) noexcept
    {
        clear();
        copy(ptr.fCharList);
        return *this;
    }

    CharStringListPtr& operator=(const LinkedList<const char*>& list) noexcept
    {
        clear();
        copy(list);
        return *this;
    }

    // -------------------------------------------------------------------

protected:
    void clear() noexcept
    {
        if (fCharList == nullptr)
            return;

        for (int i=0; fCharList[i] != nullptr; ++i)
            delete[] fCharList[i];

        delete[] fCharList;
        fCharList = nullptr;
    }

    void copy(const char* const* const c) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(c != nullptr,);
        CARLA_SAFE_ASSERT_RETURN(fCharList == nullptr,);

        std::size_t count = 0;
        for (; c[count] != nullptr; ++count) {}
        CARLA_SAFE_ASSERT_RETURN(count > 0,);

        const char** tmpList;

        try {
            tmpList = new const char*[count+1];
        } CARLA_SAFE_EXCEPTION_RETURN("CharStringListPtr::copy",);

        tmpList[count] = nullptr;

        for (std::size_t i=0; i<count; ++i)
        {
            tmpList[i] = carla_strdup_safe(c[i]);
            CARLA_SAFE_ASSERT_BREAK(tmpList[i] != nullptr);
        }

        fCharList = tmpList;
    }

    void copy(const LinkedList<const char*>& list) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(fCharList == nullptr,);

        const std::size_t count(list.count());
        CARLA_SAFE_ASSERT_RETURN(count > 0,);

        const char** tmpList;

        try {
            tmpList = new const char*[count+1];
        } CARLA_SAFE_EXCEPTION_RETURN("CharStringListPtr::copy",);

        tmpList[count] = nullptr;

        std::size_t i=0;
        for (LinkedList<const char*>::Itenerator it = list.begin2(); it.valid(); it.next(), ++i)
        {
            tmpList[i] = carla_strdup_safe(it.getValue(nullptr));
            CARLA_SAFE_ASSERT_BREAK(tmpList[i] != nullptr);
        }
        CARLA_SAFE_ASSERT(i == count);

        fCharList = tmpList;
    }

    // -------------------------------------------------------------------

private:
    const char* const* fCharList;
};

// -----------------------------------------------------------------------
// CarlaStringList

class CarlaStringList : public LinkedList<const char*>
{
public:
    CarlaStringList(bool allocateElements = true) noexcept
        : LinkedList<const char*>(),
          fAllocateElements(allocateElements) {}

    CarlaStringList(const CarlaStringList& list) noexcept
        : LinkedList<const char*>(),
          fAllocateElements(list.fAllocateElements)
    {
        for (Itenerator it = list.begin2(); it.valid(); it.next())
            LinkedList<const char*>::append(carla_strdup_safe(it.getValue(nullptr)));
    }

    ~CarlaStringList() noexcept override
    {
        clear();
    }

    // -------------------------------------------------------------------

    void clear() noexcept
    {
        if (fAllocateElements)
        {
            for (Itenerator it = begin2(); it.valid(); it.next())
            {
                if (const char* const string = it.getValue(nullptr))
                    delete[] string;
            }
        }

        LinkedList<const char*>::clear();
    }

    // -------------------------------------------------------------------

    bool append(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (const char* const stringDup = fAllocateElements ? carla_strdup_safe(string) : string)
        {
            if (LinkedList<const char*>::append(stringDup))
                return true;
            delete[] stringDup;
        }

        return false;
    }

    bool appendUnique(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (contains(string))
            return false;

        return append(string);
    }

    bool appendAt(const char* const string, const Itenerator& it) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (const char* const stringDup = fAllocateElements ? carla_strdup_safe(string) : string)
        {
            if (LinkedList<const char*>::appendAt(stringDup, it))
                return true;
            delete[] stringDup;
        }

        return false;
    }

    bool insert(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (const char* const stringDup = fAllocateElements ? carla_strdup_safe(string) : string)
        {
            if (LinkedList<const char*>::insert(stringDup))
                return true;
            delete[] stringDup;
        }

        return false;
    }

    bool insertAt(const char* const string, const Itenerator& it) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (const char* const stringDup = fAllocateElements ? carla_strdup_safe(string) : string)
        {
            if (LinkedList<const char*>::insertAt(stringDup, it))
                return true;
            delete[] stringDup;
        }

        return false;
    }

    // -------------------------------------------------------------------

    const char* getAt(const std::size_t index) const noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(fCount > 0 && index < fCount, nullptr);

        std::size_t i = 0;
        ListHead* entry = fQueue.next;

        for (; i++ != index; entry = entry->next) {}

        const Data* const data(list_entry_const(entry, Data, siblings));
        CARLA_SAFE_ASSERT_RETURN(data != nullptr, nullptr);

        return data->value;
    }

    const char* getFirst() const noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(fCount > 0, nullptr);

        const Data* const data(list_entry_const(fQueue.next, Data, siblings));
        CARLA_SAFE_ASSERT_RETURN(data != nullptr, nullptr);

        return data->value;
    }

    const char* getLast() const noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(fCount > 0, nullptr);

        const Data* const data(list_entry_const(fQueue.prev, Data, siblings));
        CARLA_SAFE_ASSERT_RETURN(data != nullptr, nullptr);

        return data->value;
    }

    // -------------------------------------------------------------------

    const char* getAndRemoveFirst() noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(fCount > 0, nullptr);

        const Data* const data = list_entry_const(fQueue.next, Data, siblings);
        CARLA_SAFE_ASSERT_RETURN(data != nullptr, nullptr);

        const char* const ret = data->value;

        Itenerator it = begin2();
        LinkedList<const char*>::remove(it);

        return ret;
    }

    // -------------------------------------------------------------------

    bool contains(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        if (fCount == 0)
            return false;

        for (Itenerator it = begin2(); it.valid(); it.next())
        {
            const char* const stringComp(it.getValue(nullptr));
            CARLA_SAFE_ASSERT_CONTINUE(stringComp != nullptr);

            if (std::strcmp(string, stringComp) == 0)
                return true;
        }

        return false;
    }

    const char* containsAndReturnString(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, nullptr);

        if (fCount == 0)
            return nullptr;

        for (Itenerator it = begin2(); it.valid(); it.next())
        {
            const char* const stringComp(it.getValue(nullptr));
            CARLA_SAFE_ASSERT_CONTINUE(stringComp != nullptr);

            if (std::strcmp(string, stringComp) == 0)
                return stringComp;
        }

        return nullptr;
    }

    // -------------------------------------------------------------------

    void remove(Itenerator& it) noexcept
    {
        if (const char* const string = it.getValue(nullptr))
            delete[] string;

        LinkedList<const char*>::remove(it);
    }

    bool removeOne(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr, false);

        for (Itenerator it = begin2(); it.valid(); it.next())
        {
            const char* const stringComp(it.getValue(nullptr));
            CARLA_SAFE_ASSERT_CONTINUE(stringComp != nullptr);

            if (std::strcmp(string, stringComp) != 0)
                continue;

            delete[] stringComp;
            LinkedList<const char*>::remove(it);
            return true;
        }

        return false;
    }

    void removeAll(const char* const string) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(string != nullptr,);

        for (Itenerator it = begin2(); it.valid(); it.next())
        {
            const char* const stringComp(it.getValue(nullptr));
            CARLA_SAFE_ASSERT_CONTINUE(stringComp != nullptr);

            if (std::strcmp(string, stringComp) != 0)
                continue;

            delete[] stringComp;
            LinkedList<const char*>::remove(it);
        }
    }

    // -------------------------------------------------------------------

    CharStringListPtr toCharStringListPtr() const noexcept
    {
        return CharStringListPtr(*this);
    }

    CarlaStringList& operator=(const char* const* const charStringList) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(! fAllocateElements, *this);

        clear();

        CARLA_SAFE_ASSERT_RETURN(charStringList != nullptr, *this);

        for (int i=0; charStringList[i] != nullptr; ++i)
        {
            if (const char* const string = carla_strdup_safe(charStringList[i]))
                LinkedList<const char*>::append(string);
        }

        return *this;
    }

    CarlaStringList& operator=(const CarlaStringList& list) noexcept
    {
        CARLA_SAFE_ASSERT_RETURN(! fAllocateElements, *this);

        clear();

        for (Itenerator it = list.begin2(); it.valid(); it.next())
        {
            if (const char* const string = carla_strdup_safe(it.getValue(nullptr)))
                LinkedList<const char*>::append(string);
        }

        return *this;
    }

private:
    const bool fAllocateElements;

    CARLA_PREVENT_VIRTUAL_HEAP_ALLOCATION
};

// -----------------------------------------------------------------------

#endif // CARLA_STRING_LIST_HPP_INCLUDED
