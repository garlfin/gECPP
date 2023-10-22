#extension GL_ARB_bindless_texture : enable

#ifdef GL_ARB_bindless_texture
    #define BINDLESS_TEXTURE(TYPE, NAME) TYPE NAME
#else
    #define BINDLESS_TEXTURE(TYPE, NAME) vec2 _pad##__LINE__
#endif
