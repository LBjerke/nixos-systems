/*
  ==============================================================================

   This file is part of the Water library.
   Copyright (c) 2016 ROLI Ltd.
   Copyright (C) 2017-2018 Filipe Coelho <falktx@falktx.com>

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

  ==============================================================================
*/

#ifndef WATER_FILEINPUTSOURCE_H_INCLUDED
#define WATER_FILEINPUTSOURCE_H_INCLUDED

#include "../files/File.h"

namespace water {

//==============================================================================
/**
    A type of InputSource that represents a normal file.

    @see InputSource
*/
class FileInputSource
{
public:
    //==============================================================================
    /** Creates a FileInputSource for a file. */
    FileInputSource (const File& file);

    /** Destructor. */
    ~FileInputSource();

    InputStream* createInputStream();
    InputStream* createInputStreamFor (const String& relatedItemPath);

private:
    //==============================================================================
    const File file;

    CARLA_DECLARE_NON_COPY_CLASS (FileInputSource)
};

}

#endif // WATER_FILEINPUTSOURCE_H_INCLUDED
