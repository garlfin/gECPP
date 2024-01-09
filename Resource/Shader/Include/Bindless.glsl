#extension GL_ARB_bindless_texture : enable

#define TEXTURE_HANDLE uvec2

#ifdef EXT_BINDLESS
    #define BINDLESS_TEXTURE(TYPE, NAME) TYPE NAME
#else
    #define BINDLESS_TEXTURE(TYPE, NAME) vec2 _pad##NAME
#endif
