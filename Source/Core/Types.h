#pragma once

#include "HelperMacros.h"

#include <cstdint>
#include <cmath>
#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <half/half.hpp>
#include <limits>

namespace Noble
{
	// Integer types

	typedef std::int8_t I8;
	typedef std::uint8_t U8;
	typedef std::int16_t I16;
	typedef std::uint16_t U16;
	typedef std::int32_t I32;
	typedef std::uint32_t U32;
	typedef std::int64_t I64;
	typedef std::uint64_t U64;

	// Byte types for buffers and such
	typedef char Byte;
	static_assert(sizeof(Byte) == 1 && std::is_signed<Byte>::value);
	typedef unsigned char UByte;
	static_assert(sizeof(UByte) == 1 && !std::is_signed<UByte>::value);

	// Floating point
	typedef half_float::half F16;
	typedef float F32;
	typedef double F64;

	// Vector/Matrix types
	typedef glm::vec<2, F32> Vector2f;
	typedef glm::vec<3, F32> Vector3f;
	typedef glm::vec<4, F32> Vector4f;

	typedef glm::vec<2, I32> Vector2i;
	typedef glm::vec<3, I32> Vector3i;
	typedef glm::vec<4, I32> Vector4i;

	typedef glm::vec<2, U32> Vector2u;
	typedef glm::vec<3, U32> Vector3u;
	typedef glm::vec<4, U32> Vector4u;

	typedef glm::vec<2, F64> Vector2d;
	typedef glm::vec<3, F64> Vector3d;
	typedef glm::vec<4, F64> Vector4d;

	typedef glm::mat<3, 3, F32> Matrix3x3f;
	typedef glm::mat<4, 4, F32> Matrix4x4f;

	// Quaternions
	typedef glm::fquat Quaternion;
	typedef glm::qua<F16> HalfQuaternion;

	/**
	 * Simple yaw-pitch-roll rotation structure
	 */
	struct Rotator
	{
		union
		{
			struct
			{
				// Y rotation
				F32 Yaw;
				// X rotation
				F32 Pitch;
				// Z rotation
				F32 Roll;
			};
			F32 Data[3];
			Vector3f AsVector;
		};

		FORCEINLINE Rotator(F32 yaw, F32 pitch, F32 roll)
			: Yaw(yaw), Pitch(pitch), Roll(roll)
		{}

		/**
		 * Returns a Quaternion representation of this Rotator
		 */
		FORCEINLINE Quaternion GetQuaternion() const
		{
			return Quaternion(AsVector);
		}

		/**
		 * Returns a 3x3 matrix representation of this Rotator
		 */
		FORCEINLINE Matrix3x3f GetMatrix3x3() const
		{
			return glm::orientate3(AsVector);
		}

		/**
		 * Returns a 4x4 matrix representation of this Rotator
		 */
		FORCEINLINE Matrix4x4f GetMatrix4x4() const
		{
			return glm::orientate4(AsVector);
		}
	};

	/**
	 * Describes a full transform (position, rotation, and scale)
	 * in 3 dimensions
	 */
	struct Transform
	{
		Vector3f Position;
		Quaternion Rotation;
		Vector3f Scale;

		Transform()
			: Position(0, 0, 0),
			Rotation(glm::identity<Quaternion>()),
			Scale(1, 1, 1)
		{}

		Transform(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale)
			: Position(pos), Rotation(rot), Scale(scale)
		{}

		/**
		 * Converts this transform into a 4x4 matrix
		 */
		Matrix4x4f GetMatrix4x4() const
		{
			return glm::translate(Position) * (Matrix4x4f)Rotation * glm::scale(Scale);
		}
	};

#ifdef NOBLE_BUILD_64BIT
	typedef U64 Size;
#else
	typedef U32 Size;
#endif

	const Size SizeMaxValue = std::numeric_limits<Size>::max();

#ifdef NOBLE_DEBUG

	STATIC_CHECK(
		sizeof(Size) == sizeof(void*),
		"Size type must be equal in size to pointer");

	// For my own sanity, verify the size of float types

	STATIC_CHECK(
		sizeof(F16) == 2,
		"F16 not 2 bytes");

	STATIC_CHECK(
		sizeof(F32) == 4,
		"F32 not 4 bytes");

	STATIC_CHECK(
		sizeof(F64) == 8,
		"F64 not 8 bytes");

#endif
}