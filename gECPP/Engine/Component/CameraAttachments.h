//
// Created by scion on 11/28/2023.
//

#pragma once
#include "GL/Texture/TextureSettings.h"

#define GE_MAX_ATTACHMENTS 2

namespace gE
{
	struct AttachmentSettings
	{
		GL::SizelessTextureSettings Attachments[GE_MAX_ATTACHMENTS];

		AttachmentSettings& operator|=(const AttachmentSettings& o)
		{
			for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
				if(o.Attachments[i])
				{
					if(Attachments[i]) GE_ASSERT(Attachments[i] == o.Attachments[i], "CONFLICTING COLOR ATTACHMENTS!");
					Attachments[i] = o.Attachments[i];
				}

			return *this;
		}

		inline AttachmentSettings operator|(const AttachmentSettings& o) const
		{
			if(this == &o) return *this;
			// Create a copy
			AttachmentSettings settings = *this;
			return settings |= o;
		};
	};
}