//
// Created by scion on 10/25/2023.
//

#include "Shader.h"
#include <GL/Texture/Texture.h>

#define INCLUDE_DIRECTIVE "#include "
#define EXTENSION_DIRECTIVE "#extension "

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>*, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&, gETF::SerializationBuffer&, gETF::SerializationBuffer& idBuffer);
	const char* GetIncludePath(const char* origin, const char* include);

	void CompileDirectives(const Array<PreprocessorPair>* pairs, gETF::SerializationBuffer& buf)
	{

		if(!pairs) return;
		for(u64 i = 0; i < pairs->Size(); i++) (*pairs)[i].WriteDirective(buf);
	}

	void CompileIncludes(const char* file, gETF::SerializationBuffer& dstBuffer, gETF::SerializationBuffer& directivesBuffer, gETF::SerializationBuffer& idBuffer)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		char* source = (char*) ReadFile(file);
		if(!source) return;
		char* line = source;

		if(idBuffer.Find(file))
		{
			LOG(file << " already included, skipping.\n");
			return;
		}
		idBuffer.StrCat(file);

		do
		{
			if(strcmpb(line, INCLUDE_DIRECTIVE))
			{
				const char* includePath = GetIncludePath(file, &line[9]);
				CompileIncludes(includePath, dstBuffer, directivesBuffer, idBuffer);
				delete[] includePath;
			}
			else if(strcmpb(line, EXTENSION_DIRECTIVE)) directivesBuffer.StrCat(line, true, '\n');
			else dstBuffer.StrCat(line, true, '\n');
		} while((line = (char*) IncrementLine(line)));

		delete[] source;
	}

	const char* GetIncludePath(const char* origin, const char* include)
	{

		if(*include == '<') return strdupc(include + 1, '>');
		else if(*include == '"')
		{
			// Allocation
			size_t filelessLen = strlencLast(origin, '/') + 1;
			size_t includeLen = strlenc(include + 1, '"');
			size_t totalLen = filelessLen + includeLen;

			char* path = new char[totalLen + 1]{};

			// Original Copy
			memcpy(path, origin, filelessLen);
			memcpy(path + filelessLen, include + 1, includeLen);

			// Canonicalization
			// TODO
			// Fr lazy atm

			return path;
		}

		std::cout << "Invalid delimiter: " << *include << '\n';
		return nullptr;
	}

	PreprocessorPair::PreprocessorPair(const char* n, const char* v)
	{

		u32 totalLength = strlen(n) + 1;
		u32 nameLength = totalLength, valueLength;

		if(v) totalLength += valueLength = strlen(v) + 1;

		Name = new char[totalLength];
		Value = nullptr;
		memcpy(Name, n, nameLength);

		if(!v) return;

		Value = Name + nameLength + 1;
		memcpy(Value, v, valueLength);
	}

	PreprocessorPair::PreprocessorPair(const PreprocessorPair& o)
		:
		Name(), Value(Name + (o.Value - o.Name))
	{

		u32 len = strlen(o.Name) + 1;
		if(o.Value) len += strlen(o.Value) + 1;
		else Value = nullptr;

		Name = new char[len];
		memcpy(Name, o.Name, len);
	}

	void PreprocessorPair::WriteDirective(gETF::SerializationBuffer& buf) const
	{

		if(!Name) return;

		buf.StrCat("#define ");
		buf.StrCat(Name);

		if(Value)
		{
			buf.Push(' ');
			buf.StrCat(Value);
		}

		buf.Push('\n');
	}

	const char* ShaderStageDefine(ShaderStageType type)
	{

		switch(type)
		{
		case Fragment: return "#define FRAGMENT_SHADER\n";
		case Vertex: return "#define VERTEX_SHADER\n";
		case Compute: return "#define COMPUTE_SHADER\n";
		default: return "";
		}
	}
}