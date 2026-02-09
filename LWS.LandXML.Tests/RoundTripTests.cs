using LWS.Geometry;

namespace LWS.LandXML.Tests;

public class RoundTripTests : IDisposable
{
    readonly string _outputDir;

    public RoundTripTests()
    {
        _outputDir = Path.Combine(AppContext.BaseDirectory, "output");
        Directory.CreateDirectory(_outputDir);
    }

    public void Dispose() { }

    [Fact]
    public void PviProfile_RoundTrips()
    {
        var doc = new LandXmlDocument
        {
            ProjectName = "PVI Test",
            LinearUnit = "meter"
        };

        var alignment = new Alignment
        {
            Name = "Road1",
            StartStation = 0,
            Elements =
            [
                new CoordGeomLine(new Pt2d(100, 200), new Pt2d(300, 200)),
                new CoordGeomLine(new Pt2d(300, 200), new Pt2d(500, 400))
            ]
        };
        alignment.Profile = new Profile
        {
            Name = "Road1 Profile",
            PVIs =
            [
                new PVI(0, 100),
                new PVI(100, 102),
                new PVI(200, 101),
                new PVI(300, 105)
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "pvi_profile.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);

        Assert.Equal("PVI Test", loaded.ProjectName);
        Assert.Equal("meter", loaded.LinearUnit);
        Assert.Single(loaded.Alignments);

        var a = loaded.Alignments[0];
        Assert.Equal("Road1", a.Name);
        Assert.Equal(0, a.StartStation);
        Assert.Equal(2, a.Elements.Count);

        // Verify coordinates (LandXML stores as Northing Easting, parser swaps back)
        Assert.Equal(100, a.Elements[0].Start.X, 6);
        Assert.Equal(200, a.Elements[0].Start.Y, 6);
        Assert.Equal(300, a.Elements[0].End.X, 6);
        Assert.Equal(200, a.Elements[0].End.Y, 6);

        Assert.NotNull(a.Profile);
        Assert.Equal("Road1 Profile", a.Profile.Name);
        Assert.Equal(4, a.Profile.PVIs.Count);
        Assert.False(a.Profile.HasElements);

        Assert.Equal(0, a.Profile.PVIs[0].Station);
        Assert.Equal(100, a.Profile.PVIs[0].Elevation);
        Assert.Equal(300, a.Profile.PVIs[3].Station);
        Assert.Equal(105, a.Profile.PVIs[3].Elevation);
    }

    [Fact]
    public void CurveElement_RoundTrips()
    {
        var doc = new LandXmlDocument
        {
            ProjectName = "Curve Test",
            LinearUnit = "meter"
        };

        // Line → CCW arc (90°) → Line, tangent at both junctions.
        // Center (0,0), R=100.
        // Arc start (100,0) angle=0, CCW tangent = (0,1) → incoming line direction (0,1).
        // Arc end (0,100) angle=π/2, CCW tangent = (-1,0) → outgoing line direction (-1,0).
        var alignment = new Alignment
        {
            Name = "CurveRoad",
            StartStation = 0,
            Elements =
            [
                new CoordGeomLine(new Pt2d(100, -200), new Pt2d(100, 0)),
                new CoordGeomCurve(
                    new Pt2d(100, 0),
                    new Pt2d(0, 100),
                    new Pt2d(0, 0),
                    RotationDirection.CCw),
                new CoordGeomLine(new Pt2d(0, 100), new Pt2d(-200, 100))
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "curve_element.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);

        Assert.Single(loaded.Alignments);
        var a = loaded.Alignments[0];
        Assert.Equal("CurveRoad", a.Name);
        Assert.Equal(3, a.Elements.Count);

        // First element: line going up, tangent to arc at (100,0)
        var line0 = Assert.IsType<CoordGeomLine>(a.Elements[0]);
        Assert.Equal(100, line0.Start.X, 6);
        Assert.Equal(-200, line0.Start.Y, 6);
        Assert.Equal(100, line0.End.X, 6);
        Assert.Equal(0, line0.End.Y, 6);
        Assert.Equal(200, line0.Length, 6);

        // Second element: CCW 90° arc
        var curve = Assert.IsType<CoordGeomCurve>(a.Elements[1]);
        Assert.Equal(100, curve.Start.X, 6);
        Assert.Equal(0, curve.Start.Y, 6);
        Assert.Equal(0, curve.End.X, 6);
        Assert.Equal(100, curve.End.Y, 6);
        Assert.Equal(0, curve.Center.X, 6);
        Assert.Equal(0, curve.Center.Y, 6);
        Assert.Equal(RotationDirection.CCw, curve.Rotation);
        Assert.Equal(100, curve.Radius, 6);
        // 90-degree arc at radius 100 → length = π/2 * 100
        Assert.Equal(Math.PI / 2 * 100, curve.Length, 4);

        // Third element: line going left, tangent to arc at (0,100)
        var line1 = Assert.IsType<CoordGeomLine>(a.Elements[2]);
        Assert.Equal(0, line1.Start.X, 6);
        Assert.Equal(100, line1.Start.Y, 6);
        Assert.Equal(-200, line1.End.X, 6);
        Assert.Equal(100, line1.End.Y, 6);
        Assert.Equal(200, line1.Length, 6);

        // Total alignment length = 200 + π/2*100 + 200
        Assert.Equal(200 + Math.PI / 2 * 100 + 200, a.Length, 4);
    }

    [Fact]
    public void CwCurveElement_RoundTrips()
    {
        var doc = new LandXmlDocument { ProjectName = "CW Curve Test" };

        // Line → CW arc (90°) → Line, tangent at both junctions.
        // Mirror of the CCW test: both approach heading north, but this one
        // turns RIGHT. Center is to the RIGHT (east) of travel.
        // Center (100,0), R=100.
        // Arc start (0,0) angle=π from center, CW tangent = (0,1) → incoming line heads north.
        // Arc end (100,100) angle=π/2 from center, CW tangent = (1,0) → outgoing line heads east.
        var alignment = new Alignment
        {
            Name = "CwRoad",
            StartStation = 50,
            Elements =
            [
                new CoordGeomLine(new Pt2d(0, -200), new Pt2d(0, 0)),
                new CoordGeomCurve(
                    new Pt2d(0, 0),
                    new Pt2d(100, 100),
                    new Pt2d(100, 0),
                    RotationDirection.Cw),
                new CoordGeomLine(new Pt2d(100, 100), new Pt2d(300, 100))
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "cw_curve_element.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);
        var a = loaded.Alignments[0];
        Assert.Equal("CwRoad", a.Name);
        Assert.Equal(3, a.Elements.Count);

        // First element: line going north, tangent to arc at (0,0)
        var line0 = Assert.IsType<CoordGeomLine>(a.Elements[0]);
        Assert.Equal(0, line0.Start.X, 6);
        Assert.Equal(-200, line0.Start.Y, 6);
        Assert.Equal(0, line0.End.X, 6);
        Assert.Equal(0, line0.End.Y, 6);

        // Second element: CW 90° arc, center to the right of travel
        var curve = Assert.IsType<CoordGeomCurve>(a.Elements[1]);
        Assert.Equal(100, curve.Center.X, 6);
        Assert.Equal(0, curve.Center.Y, 6);
        Assert.Equal(RotationDirection.Cw, curve.Rotation);
        Assert.Equal(100, curve.Radius, 6);
        Assert.Equal(Math.PI / 2 * 100, curve.Length, 4);

        // Third element: line going east, tangent to arc at (100,100)
        var line1 = Assert.IsType<CoordGeomLine>(a.Elements[2]);
        Assert.Equal(100, line1.Start.X, 6);
        Assert.Equal(100, line1.Start.Y, 6);
        Assert.Equal(300, line1.End.X, 6);
        Assert.Equal(100, line1.End.Y, 6);

        // Total length = 200 + π/2*100 + 200
        Assert.Equal(200 + Math.PI / 2 * 100 + 200, a.Length, 4);
        Assert.Equal(50, a.StartStation);
    }

    [Fact]
    public void CrossSections_RoundTrip()
    {
        var doc = new LandXmlDocument { ProjectName = "XS Test" };

        var alignment = new Alignment
        {
            Name = "Road2",
            StartStation = 0,
            Elements = [new CoordGeomLine(new Pt2d(0, 0), new Pt2d(200, 0))],
            CrossSections =
            [
                new CrossSect
                {
                    Station = 50,
                    Surfaces =
                    [
                        new CrossSectSurf
                        {
                            Name = "Ground",
                            Points =
                            [
                                new CrossSectPnt(-10, 98),
                                new CrossSectPnt(-5, 99.5),
                                new CrossSectPnt(0, 100),
                                new CrossSectPnt(5, 99.5),
                                new CrossSectPnt(10, 98)
                            ]
                        },
                        new CrossSectSurf
                        {
                            Name = "Design",
                            Points =
                            [
                                new CrossSectPnt(-6, 100),
                                new CrossSectPnt(0, 100),
                                new CrossSectPnt(6, 100)
                            ]
                        }
                    ]
                },
                new CrossSect
                {
                    Station = 150,
                    Surfaces =
                    [
                        new CrossSectSurf
                        {
                            Name = "Ground",
                            Points =
                            [
                                new CrossSectPnt(-10, 95),
                                new CrossSectPnt(0, 97),
                                new CrossSectPnt(10, 95)
                            ]
                        }
                    ]
                }
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "cross_sections.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);

        var a = loaded.Alignments[0];
        Assert.Equal(2, a.CrossSections.Count);

        var cs0 = a.CrossSections[0];
        Assert.Equal(50, cs0.Station);
        Assert.Equal(2, cs0.Surfaces.Count);
        Assert.Equal("Ground", cs0.Surfaces[0].Name);
        Assert.Equal(5, cs0.Surfaces[0].Points.Count);
        Assert.Equal(-10, cs0.Surfaces[0].Points[0].Offset);
        Assert.Equal(98, cs0.Surfaces[0].Points[0].Elevation);
        Assert.Equal("Design", cs0.Surfaces[1].Name);
        Assert.Equal(3, cs0.Surfaces[1].Points.Count);

        var cs1 = a.CrossSections[1];
        Assert.Equal(150, cs1.Station);
        Assert.Single(cs1.Surfaces);
    }

    [Fact]
    public void ElementProfile_Tangent_RoundTrips()
    {
        var doc = new LandXmlDocument { ProjectName = "Tangent Test" };

        var alignment = new Alignment
        {
            Name = "A1",
            StartStation = 0,
            Elements = [new CoordGeomLine(new Pt2d(0, 0), new Pt2d(500, 0))]
        };
        alignment.Profile = new Profile
        {
            Name = "A1 Profile",
            Elements =
            [
                new TangentElement(0, 200, 100, 0.03),
                new TangentElement(200, 500, 106, -0.02)
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "tangent_profile.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);
        var profile = loaded.Alignments[0].Profile!;

        Assert.True(profile.HasElements);
        Assert.Equal(2, profile.Elements.Count);
        Assert.Empty(profile.PVIs);

        var t0 = Assert.IsType<TangentElement>(profile.Elements[0]);
        Assert.Equal(0, t0.StartStation);
        Assert.Equal(200, t0.EndStation);
        Assert.Equal(100, t0.StartElevation, 6);
        Assert.Equal(0.03, t0.Grade, 10);

        var t1 = Assert.IsType<TangentElement>(profile.Elements[1]);
        Assert.Equal(200, t1.StartStation);
        Assert.Equal(500, t1.EndStation);
        Assert.Equal(106, t1.StartElevation, 6);
        Assert.Equal(-0.02, t1.Grade, 10);

        // Verify evaluation
        Assert.Equal(100, profile.ElevationAt(0)!.Value, 6);
        Assert.Equal(103, profile.ElevationAt(100)!.Value, 6);
        Assert.Equal(106, profile.ElevationAt(200)!.Value, 6);
        Assert.Equal(100, profile.ElevationAt(500)!.Value, 6);
    }

    [Fact]
    public void ElementProfile_Parabolic_RoundTrips()
    {
        var doc = new LandXmlDocument { ProjectName = "Parabolic Test" };

        var alignment = new Alignment
        {
            Name = "A2",
            StartStation = 0,
            Elements = [new CoordGeomLine(new Pt2d(0, 0), new Pt2d(400, 0))]
        };
        alignment.Profile = new Profile
        {
            Name = "A2 Profile",
            Elements =
            [
                new TangentElement(0, 100, 50, 0.04),
                new ParabolicElement(100, 300, 54, 0.04, -0.02),
                new TangentElement(300, 400, 56, -0.02)
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "parabolic_profile.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);
        var profile = loaded.Alignments[0].Profile!;

        Assert.Equal(3, profile.Elements.Count);

        Assert.IsType<TangentElement>(profile.Elements[0]);
        var para = Assert.IsType<ParabolicElement>(profile.Elements[1]);
        Assert.IsType<TangentElement>(profile.Elements[2]);

        Assert.Equal(100, para.StartStation);
        Assert.Equal(300, para.EndStation);
        Assert.Equal(54, para.StartElevation, 6);
        Assert.Equal(0.04, para.StartGrade, 10);
        Assert.Equal(-0.02, para.EndGrade, 10);

        // Verify C1 continuity at junctions
        var t0 = (TangentElement)profile.Elements[0];
        Assert.Equal(t0.ElevationAt(100), para.ElevationAt(100), 6);
        Assert.Equal(t0.GradeAt(100), para.GradeAt(100), 10);

        var t1 = (TangentElement)profile.Elements[2];
        Assert.Equal(para.ElevationAt(300), t1.ElevationAt(300), 6);
        Assert.Equal(para.GradeAt(300), t1.GradeAt(300), 10);

        // Midpoint elevation: elev = 54 + 0.04*100 + (-0.06)/(2*200)*100^2 = 54 + 4 - 1.5 = 56.5
        Assert.Equal(56.5, para.ElevationAt(200), 6);
    }

    [Fact]
    public void ElementProfile_Circular_RoundTrips()
    {
        var doc = new LandXmlDocument { ProjectName = "Circular Test" };

        var alignment = new Alignment
        {
            Name = "A3",
            StartStation = 0,
            Elements = [new CoordGeomLine(new Pt2d(0, 0), new Pt2d(600, 0))]
        };
        alignment.Profile = new Profile
        {
            Name = "A3 Profile",
            Elements =
            [
                new TangentElement(0, 150, 80, 0.05),
                new CircularElement(150, 350, 87.5, 0.05, -5000),
                new TangentElement(350, 600, 91.5, 0.01)
            ]
        };
        doc.Alignments.Add(alignment);

        var path = Path.Combine(_outputDir, "circular_profile.xml");
        doc.Save(path);

        var loaded = LandXmlDocument.Load(path);
        var profile = loaded.Alignments[0].Profile!;

        Assert.Equal(3, profile.Elements.Count);

        var circ = Assert.IsType<CircularElement>(profile.Elements[1]);
        Assert.Equal(150, circ.StartStation);
        Assert.Equal(350, circ.EndStation);
        Assert.Equal(87.5, circ.StartElevation, 6);
        Assert.Equal(-5000, circ.Radius, 6);

        // Verify grade at end: startGrade + length/radius = 0.05 + 200/(-5000) = 0.05 - 0.04 = 0.01
        Assert.Equal(0.01, circ.GradeAt(350), 10);

        // C1 continuity
        var t0 = (TangentElement)profile.Elements[0];
        Assert.Equal(t0.ElevationAt(150), circ.ElevationAt(150), 6);
        Assert.Equal(t0.GradeAt(150), circ.GradeAt(150), 10);

        var t1 = (TangentElement)profile.Elements[2];
        Assert.Equal(circ.EndGrade, t1.Grade, 10);
    }

    [Fact]
    public void FullDocument_DoubleRoundTrip()
    {
        // Build a document with all features
        var doc = BuildFullDocument();

        var path1 = Path.Combine(_outputDir, "full_roundtrip_1.xml");
        doc.Save(path1);

        var loaded1 = LandXmlDocument.Load(path1);
        var path2 = Path.Combine(_outputDir, "full_roundtrip_2.xml");
        loaded1.Save(path2);

        var loaded2 = LandXmlDocument.Load(path2);

        // Compare the two loaded documents
        Assert.Equal(loaded1.ProjectName, loaded2.ProjectName);
        Assert.Equal(loaded1.LinearUnit, loaded2.LinearUnit);
        Assert.Equal(loaded1.Alignments.Count, loaded2.Alignments.Count);

        for (var i = 0; i < loaded1.Alignments.Count; i++)
        {
            var a1 = loaded1.Alignments[i];
            var a2 = loaded2.Alignments[i];

            Assert.Equal(a1.Name, a2.Name);
            Assert.Equal(a1.StartStation, a2.StartStation);
            Assert.Equal(a1.Elements.Count, a2.Elements.Count);

            for (var j = 0; j < a1.Elements.Count; j++)
            {
                Assert.Equal(a1.Elements[j].Start.X, a2.Elements[j].Start.X, 6);
                Assert.Equal(a1.Elements[j].Start.Y, a2.Elements[j].Start.Y, 6);
                Assert.Equal(a1.Elements[j].End.X, a2.Elements[j].End.X, 6);
                Assert.Equal(a1.Elements[j].End.Y, a2.Elements[j].End.Y, 6);
            }

            AssertProfilesEqual(a1.Profile, a2.Profile);
            Assert.Equal(a1.CrossSections.Count, a2.CrossSections.Count);

            for (var j = 0; j < a1.CrossSections.Count; j++)
            {
                Assert.Equal(a1.CrossSections[j].Station, a2.CrossSections[j].Station);
                Assert.Equal(a1.CrossSections[j].Surfaces.Count, a2.CrossSections[j].Surfaces.Count);
            }
        }
    }

    static LandXmlDocument BuildFullDocument()
    {
        var doc = new LandXmlDocument
        {
            ProjectName = "Full Test Project",
            LinearUnit = "meter"
        };

        // Alignment with PVI profile and cross-sections
        var road = new Alignment
        {
            Name = "MainRoad",
            StartStation = 100,
            Elements =
            [
                new CoordGeomLine(new Pt2d(1000, 2000), new Pt2d(1200, 2000)),
                new CoordGeomLine(new Pt2d(1200, 2000), new Pt2d(1400, 2100))
            ],
            CrossSections =
            [
                new CrossSect
                {
                    Station = 150,
                    Surfaces =
                    [
                        new CrossSectSurf
                        {
                            Name = "Ground",
                            Points = [new CrossSectPnt(-8, 49), new CrossSectPnt(0, 50), new CrossSectPnt(8, 49)]
                        }
                    ]
                }
            ]
        };
        road.Profile = new Profile
        {
            Name = "MainRoad VP",
            PVIs = [new PVI(100, 50), new PVI(200, 52), new PVI(350, 51)]
        };
        doc.Alignments.Add(road);

        // Alignment with element-based profile
        var access = new Alignment
        {
            Name = "AccessTrack",
            StartStation = 0,
            Elements = [new CoordGeomLine(new Pt2d(1000, 2000), new Pt2d(1000, 2300))]
        };
        access.Profile = new Profile
        {
            Name = "AccessTrack VP",
            Elements =
            [
                new TangentElement(0, 80, 50, 0.03),
                new ParabolicElement(80, 180, 52.4, 0.03, -0.01),
                new TangentElement(180, 300, 53.4, -0.01)
            ]
        };
        doc.Alignments.Add(access);

        return doc;
    }

    static void AssertProfilesEqual(Profile? p1, Profile? p2)
    {
        if (p1 == null && p2 == null) return;
        Assert.NotNull(p1);
        Assert.NotNull(p2);
        Assert.Equal(p1.Name, p2.Name);
        Assert.Equal(p1.PVIs.Count, p2.PVIs.Count);
        Assert.Equal(p1.Elements.Count, p2.Elements.Count);

        for (var i = 0; i < p1.PVIs.Count; i++)
        {
            Assert.Equal(p1.PVIs[i].Station, p2.PVIs[i].Station, 6);
            Assert.Equal(p1.PVIs[i].Elevation, p2.PVIs[i].Elevation, 6);
        }

        for (var i = 0; i < p1.Elements.Count; i++)
        {
            var e1 = p1.Elements[i];
            var e2 = p2.Elements[i];
            Assert.Equal(e1.GetType(), e2.GetType());
            Assert.Equal(e1.StartStation, e2.StartStation, 6);
            Assert.Equal(e1.EndStation, e2.EndStation, 6);
            Assert.Equal(e1.StartElevation, e2.StartElevation, 6);
            Assert.Equal(e1.StartGrade, e2.StartGrade, 10);
        }
    }
}
