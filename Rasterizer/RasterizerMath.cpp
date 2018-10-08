#include "RasterizerMath.h"

#include "Screen.h"
#include "Globals.h"

float orient2d(const Point3D& a, const Point3D& b, const ScreenCoord& c)
{
	float p = (b.x - a.x)*(c.y - a.y);
	float z = (b.y - a.y)*(c.x - a.x);
	return p - z;
}

void drawTri(const Triangle& Tri, std::function<void(ScreenCoord, const Triangle&, float, float, float)> putpixel)
{
	const Point3D v0 = Point3D(Tri.Positions[0]);
	const Point3D v1 = Point3D(Tri.Positions[1]);
	const Point3D v2 = Point3D(Tri.Positions[2]);

	// Compute triangle bounding box
	int minX = (int)std::min(v0.x, std::min(v1.x, v2.x));
	int minY = (int)std::min(v0.y, std::min(v1.y, v2.y));
	int maxX = (int)std::max(v0.x, std::max(v1.x, v2.x));
	int maxY = (int)std::max(v0.y, std::max(v1.y, v2.y));

	// Clip against screen bounds
	minX = std::max(minX, 0);
	minY = std::max(minY, 0);
	maxX = std::min(maxX, ScreenWidth - 1);
	maxY = std::min(maxY, ScreenHeight - 1);

	// Rasterize
	ScreenCoord p;


	for (p.y = minY; p.y <= maxY; p.y++) {
		for (p.x = minX; p.x <= maxX; p.x++) {
			// Determine barycentric coordinates
			const float w0 = orient2d(v1, v2, p);
			const float w1 = orient2d(v2, v0, p);
			const float w2 = orient2d(v0, v1, p);
			float area = /*0.5 */(w0 + w1 + w2);

			// If p is on or inside all edges, render pixel.
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				putpixel(p, Tri, w0 / area, w1 / area, w2 / area);
		}
	}
}

void drawTri_Tile(const Triangle& Tri, FramebufferTile * Tile)
{
	const Point3D v0 = Point3D(Tri.Positions[0]);
	const Point3D v1 = Point3D(Tri.Positions[1]);
	const Point3D v2 = Point3D(Tri.Positions[2]);

	// Compute triangle bounding box
	int minX = (int)std::min(v0.x, std::min(v1.x, v2.x));
	int minY = (int)std::min(v0.y, std::min(v1.y, v2.y));
	int maxX = (int)std::max(v0.x, std::max(v1.x, v2.x));
	int maxY = (int)std::max(v0.y, std::max(v1.y, v2.y));

	// Clip against tile bounds
	minX = std::max(minX, (int)Tile->minX);
	minY = std::max(minY, (int)Tile->minY);
	maxX = std::min(maxX, (int)Tile->maxX - 1);
	maxY = std::min(maxY, (int)Tile->maxY - 1);

	// Rasterize
	ScreenCoord p;


	for (p.y = minY; p.y <= maxY; p.y++) {
		for (p.x = minX; p.x <= maxX; p.x++) {
			// Determine barycentric coordinates
			float w0 = orient2d(v1, v2, p);
			float w1 = orient2d(v2, v0, p);
			float w2 = orient2d(v0, v1, p);
			float area = /*0.5 */(w0 + w1 + w2);

			// If p is on or inside all edges, render pixel.
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				w0 /= area;
				w1 /= area;
				w2 /= area;
				glm::vec3 color = w0 * Tri.Normals[0] + w1 * Tri.Normals[1] + w2 * Tri.Normals[2];
				//color = Tri.Colors[0];//w0 * Tri.Colors[0] + w1 * Tri.Colors[1] + w2 * Tri.Colors[2];
				float depth = 1.0 / (w0 * Tri.Positions[0].z + w1 * Tri.Positions[1].z + w2 * Tri.Positions[2].z);
				//Color Red((char)255,0,0);
				//Red.r = 255;
				//Color RandomColor(  ) ;
				g_Framebuffer->SetPixel(p.x, p.y, Color(color.r, color.g, color.b), depth);
			}
				//putpixel(p, Tri, w0 / area, w1 / area, w2 / area);
		}
	}
}
