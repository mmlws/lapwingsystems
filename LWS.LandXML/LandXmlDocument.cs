using System.Globalization;
using System.Xml.Linq;
using LWS.Geometry;

namespace LWS.LandXML;

public class LandXmlDocument
{
    public string ProjectName { get; set; } = "Untitled";
    public string LinearUnit { get; set; } = "meter";
    public List<Alignment> Alignments { get; } = [];

    public static LandXmlDocument Load(string filePath)
    {
        var xdoc = XDocument.Load(filePath);
        return Load(xdoc);
    }

    public static LandXmlDocument Load(Stream stream)
    {
        var xdoc = XDocument.Load(stream);
        return Load(xdoc);
    }

    static LandXmlDocument Load(XDocument xdoc)
    {
        var root = xdoc.Root ?? throw new InvalidOperationException("Missing root element");
        var doc = new LandXmlDocument();

        // Project name
        var projectEl = root.Element("Project");
        if (projectEl != null)
            doc.ProjectName = (string?)projectEl.Attribute("name") ?? "Untitled";

        // Units
        var metricEl = root.Element("Units")?.Element("Metric");
        if (metricEl != null)
            doc.LinearUnit = (string?)metricEl.Attribute("linearUnit") ?? "meter";

        // Alignments
        var alignmentsEl = root.Element("Alignments");
        if (alignmentsEl != null)
        {
            foreach (var alignEl in alignmentsEl.Elements("Alignment"))
                doc.Alignments.Add(ParseAlignment(alignEl));
        }

        return doc;
    }

    static Alignment ParseAlignment(XElement alignEl)
    {
        var name = (string?)alignEl.Attribute("name") ?? "Unnamed";
        var staStart = ParseDouble((string?)alignEl.Attribute("staStart") ?? "0");

        var lines = new List<CoordGeomLine>();
        var coordGeomEl = alignEl.Element("CoordGeom");
        if (coordGeomEl != null)
        {
            foreach (var lineEl in coordGeomEl.Elements("Line"))
            {
                var start = ParsePoint(lineEl.Element("Start")!.Value);
                var end = ParsePoint(lineEl.Element("End")!.Value);
                lines.Add(new CoordGeomLine(start, end));
            }
        }

        var alignment = new Alignment
        {
            Name = name,
            StartStation = staStart,
            Lines = lines
        };

        // Profile
        var profileEl = alignEl.Element("Profile");
        if (profileEl != null)
        {
            var profAlignEl = profileEl.Element("ProfAlign");
            if (profAlignEl != null)
            {
                var profileName = (string?)profileEl.Attribute("name") ?? name;
                var pvis = new List<PVI>();
                var elements = new List<ProfileElement>();

                foreach (var child in profAlignEl.Elements())
                {
                    switch (child.Name.LocalName)
                    {
                        case "PVI":
                        {
                            var parts = child.Value.Trim().Split(' ', StringSplitOptions.RemoveEmptyEntries);
                            pvis.Add(new PVI(ParseDouble(parts[0]), ParseDouble(parts[1])));
                            break;
                        }
                        case "Tangent":
                            elements.Add(new TangentElement(
                                ParseDouble((string)child.Attribute("staStart")!),
                                ParseDouble((string)child.Attribute("staEnd")!),
                                ParseDouble((string)child.Attribute("elevation")!),
                                ParseDouble((string)child.Attribute("grade")!)));
                            break;
                        case "ParaCurve":
                        {
                            var staStart2 = child.Attribute("staStart");
                            if (staStart2 != null)
                            {
                                // Element-based: has staStart, staEnd, elevation, gradeIn, gradeOut
                                elements.Add(new ParabolicElement(
                                    ParseDouble((string)staStart2),
                                    ParseDouble((string)child.Attribute("staEnd")!),
                                    ParseDouble((string)child.Attribute("elevation")!),
                                    ParseDouble((string)child.Attribute("gradeIn")!),
                                    ParseDouble((string)child.Attribute("gradeOut")!)));
                            }

                            break;
                        }
                        case "CircCurve":
                        {
                            var staStart2 = child.Attribute("staStart");
                            if (staStart2 != null)
                            {
                                elements.Add(new CircularElement(
                                    ParseDouble((string)staStart2),
                                    ParseDouble((string)child.Attribute("staEnd")!),
                                    ParseDouble((string)child.Attribute("elevation")!),
                                    ParseDouble((string)child.Attribute("gradeIn")!),
                                    ParseDouble((string)child.Attribute("radius")!)));
                            }

                            break;
                        }
                    }
                }

                alignment.Profile = new Profile
                {
                    Name = profileName,
                    PVIs = pvis,
                    Elements = elements
                };
            }
        }

        // Cross-sections
        var crossSectsEl = alignEl.Element("CrossSects");
        if (crossSectsEl != null)
        {
            foreach (var csEl in crossSectsEl.Elements("CrossSect"))
            {
                var station = ParseDouble((string?)csEl.Attribute("sta") ?? "0");
                var surfaces = new List<CrossSectSurf>();
                foreach (var surfEl in csEl.Elements("CrossSectSurf"))
                {
                    var surfName = (string?)surfEl.Attribute("name") ?? "Surface";
                    var points = new List<CrossSectPnt>();
                    foreach (var ptEl in surfEl.Elements("CrossSectPnt"))
                    {
                        var parts = ptEl.Value.Trim().Split(' ', StringSplitOptions.RemoveEmptyEntries);
                        points.Add(new CrossSectPnt(ParseDouble(parts[0]), ParseDouble(parts[1])));
                    }

                    surfaces.Add(new CrossSectSurf { Name = surfName, Points = points });
                }

                alignment.CrossSections.Add(new CrossSect { Station = station, Surfaces = surfaces });
            }
        }

        return alignment;
    }

    /// <summary>Parse a LandXML point string "Northing Easting" → Pt2d(Easting, Northing).</summary>
    static Pt2d ParsePoint(string text)
    {
        var parts = text.Trim().Split(' ', StringSplitOptions.RemoveEmptyEntries);
        var northing = ParseDouble(parts[0]);
        var easting = ParseDouble(parts[1]);
        return new Pt2d(easting, northing);
    }

    static double ParseDouble(string text) =>
        double.Parse(text, NumberStyles.Float, CultureInfo.InvariantCulture);

    public XDocument ToXDocument()
    {
        var now = DateTime.Now;

        var root = new XElement("LandXML",
            new XAttribute("version", "1.2"),
            new XAttribute("date", now.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture)),
            new XAttribute("time", now.ToString("HH:mm:ss", CultureInfo.InvariantCulture)),
            new XElement("Units",
                new XElement("Metric",
                    new XAttribute("linearUnit", LinearUnit),
                    new XAttribute("areaUnit", "squareMeter"),
                    new XAttribute("volumeUnit", "cubicMeter"))),
            new XElement("Project",
                new XAttribute("name", ProjectName)));

        if (Alignments.Count > 0)
        {
            var alignmentsEl = new XElement("Alignments");
            foreach (var alignment in Alignments)
                alignmentsEl.Add(SerializeAlignment(alignment));
            root.Add(alignmentsEl);
        }

        return new XDocument(
            new XDeclaration("1.0", "utf-8", null),
            root);
    }

    public void Save(string filePath) => ToXDocument().Save(filePath);

    public void Save(Stream stream) => ToXDocument().Save(stream);

    static XElement SerializeAlignment(Alignment alignment)
    {
        var el = new XElement("Alignment",
            new XAttribute("name", alignment.Name),
            new XAttribute("length", Fmt(alignment.Length)),
            new XAttribute("staStart", Fmt(alignment.StartStation)));

        if (alignment.Lines.Count > 0)
        {
            var coordGeom = new XElement("CoordGeom");
            foreach (var line in alignment.Lines)
            {
                var dx = line.End.X - line.Start.X;
                var dy = line.End.Y - line.Start.Y;
                var len = Math.Sqrt(dx * dx + dy * dy);

                coordGeom.Add(new XElement("Line",
                    new XAttribute("length", Fmt(len)),
                    new XElement("Start", FormatPoint(line.Start.X, line.Start.Y)),
                    new XElement("End", FormatPoint(line.End.X, line.End.Y))));
            }

            el.Add(coordGeom);
        }

        if (alignment.Profile is { } profile)
            el.Add(SerializeProfile(profile));

        if (alignment.CrossSections.Count > 0)
            el.Add(SerializeCrossSects(alignment.CrossSections));

        return el;
    }

    static XElement SerializeProfile(Profile profile)
    {
        var profAlign = new XElement("ProfAlign",
            new XAttribute("name", profile.Name));

        if (profile.HasElements)
        {
            foreach (var element in profile.Elements)
                profAlign.Add(SerializeProfileElement(element));
        }
        else
        {
            foreach (var pvi in profile.PVIs)
                profAlign.Add(new XElement("PVI", $"{Fmt(pvi.Station)} {Fmt(pvi.Elevation)}"));
        }

        return new XElement("Profile",
            new XAttribute("name", profile.Name),
            profAlign);
    }

    static XElement SerializeProfileElement(ProfileElement element) => element switch
    {
        TangentElement t => new XElement("Tangent",
            new XAttribute("staStart", Fmt(t.StartStation)),
            new XAttribute("staEnd", Fmt(t.EndStation)),
            new XAttribute("elevation", Fmt(t.StartElevation)),
            new XAttribute("grade", Fmt(t.Grade))),
        ParabolicElement p => new XElement("ParaCurve",
            new XAttribute("staStart", Fmt(p.StartStation)),
            new XAttribute("staEnd", Fmt(p.EndStation)),
            new XAttribute("elevation", Fmt(p.StartElevation)),
            new XAttribute("gradeIn", Fmt(p.StartGrade)),
            new XAttribute("gradeOut", Fmt(p.EndGrade))),
        CircularElement c => new XElement("CircCurve",
            new XAttribute("staStart", Fmt(c.StartStation)),
            new XAttribute("staEnd", Fmt(c.EndStation)),
            new XAttribute("elevation", Fmt(c.StartElevation)),
            new XAttribute("gradeIn", Fmt(c.StartGrade)),
            new XAttribute("radius", Fmt(c.Radius))),
        _ => throw new NotSupportedException($"Unknown profile element type: {element.GetType().Name}")
    };

    static XElement SerializeCrossSects(List<CrossSect> crossSections)
    {
        var crossSects = new XElement("CrossSects");
        foreach (var cs in crossSections)
        {
            var csEl = new XElement("CrossSect",
                new XAttribute("sta", Fmt(cs.Station)));
            foreach (var surf in cs.Surfaces)
            {
                var surfEl = new XElement("CrossSectSurf",
                    new XAttribute("name", surf.Name));
                foreach (var pt in surf.Points)
                    surfEl.Add(new XElement("CrossSectPnt", $"{Fmt(pt.Offset)} {Fmt(pt.Elevation)}"));
                csEl.Add(surfEl);
            }

            crossSects.Add(csEl);
        }

        return crossSects;
    }

    /// <summary>Format a coordinate as "Northing Easting" (Y X) per LandXML convention.</summary>
    static string FormatPoint(double x, double y) => $"{Fmt(y)} {Fmt(x)}";

    static string Fmt(double value) => value.ToString("G", CultureInfo.InvariantCulture);
}