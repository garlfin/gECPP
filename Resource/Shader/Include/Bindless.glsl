#extension GL_ARB_bindless_texture : enable

#define TEXTURE_HANDLE vec2

#ifdef GL_ARB_bindless_texture
    #define BINDLESS_TEXTURE(TYPE, NAME) TYPE NAME
#else
    #define BINDLESS_TEXTURE(TYPE, NAME) vec2 _pad##NAME
#endif
