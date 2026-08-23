#pragma once
#include "Vertex2.h"
#include "Vector3.h"

namespace CPURenderer {
	struct CustomMath {
		static inline Vector3 GetBarycentric(
			const Vector2& a,
			const Vector2& b,
			const Vector2& c,
			const Vector2& p)
		{
			float denom =
				(b.y - c.y) * (a.x - c.x) +
				(c.x - b.x) * (a.y - c.y);

			float w0 =
				((b.y - c.y) * (p.x - c.x) +
					(c.x - b.x) * (p.y - c.y)) / denom;

			float w1 =
				((c.y - a.y) * (p.x - c.x) +
					(a.x - c.x) * (p.y - c.y)) / denom;

			float w2 = 1.0f - w0 - w1;

			return { w0, w1, w2 };
		};

		static inline std::pair<float, float> GetUVWeightsFast(
			const Vertex2& a,
			const Vertex2& b,
			const Vertex2& c,
			Vector2 h)
		{
			const float denom =
				(b.vector.y - c.vector.y) * (a.vector.x - c.vector.x) +
				(c.vector.x - b.vector.x) * (a.vector.y - c.vector.y);

			if (std::abs(denom) < 0.000001f)
				return { 0.0f, 0.0f };

			const float wa =
				((b.vector.y - c.vector.y) * (h.x - c.vector.x) +
					(c.vector.x - b.vector.x) * (h.y - c.vector.y))
				/ denom;

			const float wb =
				((c.vector.y - a.vector.y) * (h.x - c.vector.x) +
					(a.vector.x - c.vector.x) * (h.y - c.vector.y))
				/ denom;

			const float wc = 1.0f - wa - wb;

			const float u =
				wa * a.u +
				wb * b.u +
				wc * c.u;

			const float v =
				wa * a.v +
				wb * b.v +
				wc * c.v;

			return { u, v };
		};

		static inline bool IsPointInsideTriangleFast(
			Vector2 a,
			Vector2 b,
			Vector2 c,
			Vector2 h)
		{
			Vector2 AB = b - a;
			Vector2 AH = h - a;

			Vector2 BC = c - b;
			Vector2 BH = h - b;

			Vector2 CA = a - c;
			Vector2 CH = h - c;

			float cross1 = AB.Cross(AH);
			float cross2 = BC.Cross(BH);
			float cross3 = CA.Cross(CH);

			bool hasNegative =
				cross1 < 0.0f ||
				cross2 < 0.0f ||
				cross3 < 0.0f;

			bool hasPositive =
				cross1 > 0.0f ||
				cross2 > 0.0f ||
				cross3 > 0.0f;

			return !(hasNegative && hasPositive);
		};
	};
}

//static double GetDistanceToOppositeSide(double ad1, double ad2, double opposite) {
//	double ad1Sq = ad1 * ad1;
//	double p = ((opposite * opposite) + (ad1Sq)-(ad2 * ad2)) / (2.0 * opposite);
//	double len = std::sqrt(ad1Sq - (p * p));
//	return len;
//}
//
//static std::pair<float, float> GetUVWeights(Vertex2& a, Vertex2& b, Vertex2& c, Vector2 h) {
//	double ah = h.GetDistance(a.vector);
//	double bh = h.GetDistance(b.vector);
//	double ch = h.GetDistance(c.vector);
//
//	double ab = (a.vector - b.vector).GetLength();
//	double ac = (a.vector - c.vector).GetLength();
//	double bc = (b.vector - c.vector).GetLength();
//
//	double distB = GetDistanceToOppositeSide(ah, ch, (a.vector - c.vector).GetLength());
//	double distC = GetDistanceToOppositeSide(ah, bh, (a.vector - b.vector).GetLength());
//	double distA = GetDistanceToOppositeSide(ch, bh, (b.vector - c.vector).GetLength());
//
//	double heightA = GetDistanceToOppositeSide(ab, ac, bc);
//	double heightB = GetDistanceToOppositeSide(ab, bc, ac);
//	double heightC = GetDistanceToOppositeSide(bc, ac, ab);
//
//	double a1 = distA / heightA;
//	double b1 = distB / heightB;
//	double c1 = distC / heightC;
//
//	float u = a1 * a.u + b1 * b.u + c1 * c.u;
//	float v = a1 * a.v + b1 * b.v + c1 * c.v;
//
//	return { u, v };
//}

//static double FindAngleByCosineLaw(
//	double adjacentA,
//	double adjacentB,
//	double opposite)
//{
//	assert(adjacentA > 0.0);
//	assert(adjacentB > 0.0);
//
//	double cosine =
//		(adjacentA * adjacentA +
//			adjacentB * adjacentB -
//			opposite * opposite)
//		/ (2.0 * adjacentA * adjacentB);
//
//	cosine = std::clamp(cosine, -1.0, 1.0);
//
//	return std::acos(cosine);
//}