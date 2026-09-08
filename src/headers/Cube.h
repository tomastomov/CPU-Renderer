#pragma once

#include <Vector3.h>
#include <Vertex3.h>

namespace CPURenderer {
	struct Cube {
		Vector3 a;
		Vector3 b;
		Vector3 c;
		Vector3 d;
		Vector3 a1;
		Vector3 b1;
		Vector3 c1;
		Vector3 d1;
		Vector3 pos;
		Vector3 size;
		Vector3 rotate;
	};

    struct CubeMesh {
        // Front
        Vertex3 frontA;
        Vertex3 frontB;
        Vertex3 frontC;
        Vertex3 frontD;

        // Right
        Vertex3 rightA;
        Vertex3 rightB;
        Vertex3 rightC;
        Vertex3 rightD;

        // Back
        Vertex3 backA;
        Vertex3 backB;
        Vertex3 backC;
        Vertex3 backD;

        // Left
        Vertex3 leftA;
        Vertex3 leftB;
        Vertex3 leftC;
        Vertex3 leftD;

        // Top
        Vertex3 topA;
        Vertex3 topB;
        Vertex3 topC;
        Vertex3 topD;

        // Bottom
        Vertex3 bottomA;
        Vertex3 bottomB;
        Vertex3 bottomC;
        Vertex3 bottomD;

        Vector3 pos;
        Vector3 size;
        Vector3 rotate;
    };
}