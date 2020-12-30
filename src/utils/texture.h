#ifndef TEXTURE_H
#define TEXTURE_H

namespace texture
{
	unsigned int loadTexture(const char* path, bool flip);
    unsigned int loadFromFile(const char* path, unsigned int format, bool flip, int wrapping_mode, int filtering_mode);
}

#endif
