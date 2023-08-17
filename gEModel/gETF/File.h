#include <GLMath.h>
#include "Prototype.h"

#define GETF_VERSION 1
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define TRIANGLE_MODE_SIMPLE 0
#define TRIANGLE_MODE_COMPLEX 1

#define NODISCARD [[nodiscard]]
#define ALWAYS_INLINE __attribute__((always_inline))

namespace gETF
{
	struct Header;
	struct Mesh;
	struct VertexField;
	struct MaterialSlot;
	struct Serializable;
	struct SerializationBuffer;

	// TODO
	// struct Scene;
	// struct Node;

	Header Import(const char*);

	struct VertexField : public Serializable
	{
		SERIALIZABLE_PROTO;

		const char* Name;

		u8 Index;

		u32 Type;
		u8 TypeCount;

		u32 Count;

		void* Data;

		~VertexField()
		{
			delete[] (u8*)Data;
			delete[] Name;
		}
	};

	struct MaterialSlot : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialIndex;
		u32 Offset;
		u32 Count;
	};

	struct Mesh : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialCount;
		u8 FieldCount;
		u8 TriangleMode;

		VertexField* Fields;
		MaterialSlot* Materials;

		~Mesh()
		{
			delete[] Fields;
			delete[] Materials;
		}
	};

	struct Header : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MeshCount;
		Mesh* Meshes;

		~Header()
		{
			delete[] Meshes;
		}
	};
}
