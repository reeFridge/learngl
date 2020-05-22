#ifndef TEXTURE_H
#define TEXTURE_H

namespace texture
{
    unsigned int loadFromFile(const char* path, unsigned int format, bool flip, int wrapping_mode, int filtering_mode);
}

#endif
