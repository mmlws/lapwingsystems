#include <iostream>
#include <vector>
#include <cmath>
#include <fade25d/Fade_2D.h>

using namespace GEOM_FADE25D;
using namespace std;

int main()
{
    cout << "=== Fade25D Terrain Mesh Example ===" << endl;

    // Create a Fade_2D object (2.5D Delaunay triangulation)
    Fade_2D dt;

    // Generate a simple terrain mesh - a grid with varying heights
    const int gridSize = 20;
    const double spacing = 1.0;
    vector<Point2> points;

    cout << "Generating terrain points..." << endl;

    for (int i = 0; i < gridSize; ++i)
    {
        for (int j = 0; j < gridSize; ++j)
        {
            double x = i * spacing;
            double y = j * spacing;

            // Create a simple terrain with hills using a sine wave
            double z = 2.0 * sin(x * 0.5) * cos(y * 0.5) +
                      1.0 * sin(x * 0.3) * sin(y * 0.3);

            points.push_back(Point2(x, y, z));
        }
    }

    cout << "Inserting " << points.size() << " points into triangulation..." << endl;

    // Insert all points into the triangulation
    dt.insert(points);

    // Get statistics
    size_t numPoints = dt.numberOfPoints();
    size_t numTriangles = dt.numberOfTriangles();

    cout << "\nTriangulation complete:" << endl;
    cout << "  Points: " << numPoints << endl;
    cout << "  Triangles: " << numTriangles << endl;

    // Get all triangles
    vector<Triangle2*> triangles;
    dt.getTrianglePointers(triangles);

    // Display some sample triangle information
    if (triangles.size() > 0)
    {
        cout << "\nFirst triangle vertices:" << endl;
        Triangle2* tri = triangles[0];
        Point2* p0 = tri->getCorner(0);
        Point2* p1 = tri->getCorner(1);
        Point2* p2 = tri->getCorner(2);

        cout << "  V0: (" << p0->x() << ", " << p0->y() << ", " << p0->z() << ")" << endl;
        cout << "  V1: (" << p1->x() << ", " << p1->y() << ", " << p1->z() << ")" << endl;
        cout << "  V2: (" << p2->x() << ", " << p2->y() << ", " << p2->z() << ")" << endl;
    }

    // Export to OBJ file for visualization (can be opened in Rhino, Blender, etc.)
    cout << "\nExporting to terrain_output.obj..." << endl;
    dt.writeObj("terrain_output.obj");    

    cout << "\nDone! Output saved to terrain_output.obj" << endl;
    cout << "You can open this file in Rhino, Blender, or other 3D software." << endl;

    return 0;
}
