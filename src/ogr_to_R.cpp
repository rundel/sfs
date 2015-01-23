// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>




///
/// Util
///

inline void assert(bool test, const std::string& error = "Assertion Failed.")
{
    if(!test) Rcpp::stop(error);
}

Rcpp::NumericMatrix get_ogr_points(OGRGeometryH geom)
{
    assert(OGR_G_GetCoordinateDimension(geom) == 2, "Only 2d geometries allowed");

    int n = OGR_G_GetPointCount(geom);

    Rcpp::NumericMatrix coords(n,2);

    for(int i=0; i < n; ++i)
    {
        coords(i,0) = OGR_G_GetX (geom, i);
        coords(i,1) = OGR_G_GetY (geom, i);
    }

    return(coords);
}


////
//// CRS
////

Rcpp::S4 CRS(std::string s = "")
{
    Rcpp::S4 crs("CRS");
    Rcpp::CharacterVector projargs = s.empty() ? NA_STRING : Rcpp::wrap(s);
    crs.slot("projargs") = projargs;

    return(crs);
}


////
//// Point
////

Rcpp::S4 point_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbPoint, "Geometry must be a point");
    assert(OGR_G_GetGeometryCount(geom) == 0, "Subgeometries not allowed");
    assert(OGR_G_GetPointCount(geom) == 1, "Point should only have 1 coordinate");

    Rcpp::NumericMatrix coords = get_ogr_points(geom);

    Rcpp::S4 sfs("sfs_point");
    sfs.slot("coords") = coords;
    sfs.slot("data")   = Rcpp::DataFrame();
    sfs.slot("crs")    = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_point()
{
    OGRGeometryH geom;
    char* wkt = "POINT (30 10)";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = point_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Multipoint
////

Rcpp::S4 multipoint_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiPoint, "Geometry must be a multipoint");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = point_ogr_to_sfs( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multipoint");
    sfs.slot("geoms") = geoms;
    sfs.slot("data")  = Rcpp::DataFrame();
    sfs.slot("crs")   = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_multipoint()
{
    OGRGeometryH geom;
    char* wkt = "MULTIPOINT ((10 40), (40 30), (20 20), (30 10))";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = multipoint_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Linestring
////

Rcpp::S4 linestring_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbLineString, "Geometry must be a Linestring");
    assert(OGR_G_GetGeometryCount(geom) == 0, "Subgeometries not allowed");

    Rcpp::NumericMatrix coords = get_ogr_points(geom);

    Rcpp::S4 sfs("sfs_linestring");
    sfs.slot("coords") = coords;
    sfs.slot("data")   = Rcpp::DataFrame();
    sfs.slot("crs")    = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_linestring()
{
    OGRGeometryH geom;
    char* wkt = "LINESTRING (30 10, 10 30, 40 40)";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = linestring_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Multilinestring
////

Rcpp::S4 multilinestring_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiLineString, "Geometry must be a multilinestring");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = linestring_ogr_to_sfs( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multilinestring");
    sfs.slot("geoms") = geoms;
    sfs.slot("data")  = Rcpp::DataFrame();
    sfs.slot("crs")   = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_multilinestring()
{
    OGRGeometryH geom;
    char* wkt = "MULTILINESTRING ((10 10, 20 20, 10 40),(40 40, 30 30, 40 20, 30 10))";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = multilinestring_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Polygon
////

Rcpp::S4 polygon_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbPolygon, "Geometry must be a Polygon");
    assert(OGR_G_GetGeometryCount(geom) >= 1, "There must be at least 1 subgeometry");


    OGRGeometryH shell = OGR_G_GetGeometryRef(geom, 0);
    assert(OGR_G_GetGeometryType(shell) == wkbLineString, "Polygon shell should be a linestring");

    Rcpp::NumericMatrix coords = get_ogr_points(shell);


    Rcpp::List holes(OGR_G_GetGeometryCount(geom)-1);
    for(int i=1; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        OGRGeometryH hole = OGR_G_GetGeometryRef(geom, i);
        assert(OGR_G_GetGeometryType(hole) == wkbLineString, "Polygon holes should be a linestrings");

        holes[i-1] = get_ogr_points(hole);
    }


    Rcpp::S4 sfs("sfs_polygon");
    sfs.slot("coords") = coords;
    sfs.slot("holes")  = holes;
    sfs.slot("data")   = Rcpp::DataFrame();
    sfs.slot("crs")    = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_polygon(bool hole = false)
{
    OGRGeometryH geom;
    char* wkt_hole = "POLYGON ((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))";
    char* wkt      = "POLYGON ((30 10, 40 40, 20 40, 10 20, 30 10))";

    OGRErr err = hole ? OGR_G_CreateFromWkt(&wkt_hole, NULL, &geom)
                      : OGR_G_CreateFromWkt(&wkt,      NULL, &geom);

    assert(err == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = polygon_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Multipolygon
////

Rcpp::S4 multipolygon_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiPolygon, "Geometry must be a multipolygon");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = point_ogr_to_sfs( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multipolygon");
    sfs.slot("geoms") = geoms;
    sfs.slot("data")  = Rcpp::DataFrame();
    sfs.slot("crs")   = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_multipolygon(bool hole = false)
{
    OGRGeometryH geom;
    char* wkt_hole = "MULTIPOLYGON (((40 40, 20 45, 45 30, 40 40)), ((20 35, 10 30, 10 10, 30 5, 45 20, 20 35), (30 20, 20 15, 20 25, 30 20)))";
    char* wkt      = "MULTIPOINT ((10 40), (40 30), (20 20), (30 10))";

    OGRErr err = hole ? OGR_G_CreateFromWkt(&wkt_hole, NULL, &geom)
                      : OGR_G_CreateFromWkt(&wkt,      NULL, &geom);

    assert(err == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = multipolygon_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}


////
//// Geometry Collection
////

Rcpp::S4 geometrycollection_ogr_to_sfs(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbGeometryCollection, "Geometry must be a geometry collection");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        OGRGeometryH subgeom = OGR_G_GetGeometryRef(geom, i);
        
        switch(OGR_G_GetGeometryType(subgeom))
        {
            case wkbPoint:
                geoms[i] = point_ogr_to_sfs(subgeom);
                break;
            case wkbLineString:
                geoms[i] = linestring_ogr_to_sfs(subgeom);
                break;
            case wkbPolygon:
                geoms[i] = polygon_ogr_to_sfs(subgeom);
                break;
            case wkbMultiPoint:
                geoms[i] = multipoint_ogr_to_sfs(subgeom);
                break;
            case wkbMultiLineString:
                geoms[i] = multilinestring_ogr_to_sfs(subgeom);
                break;
            case wkbMultiPolygon:
                geoms[i] = multipolygon_ogr_to_sfs(subgeom);
                break;
            case wkbGeometryCollection:
                geoms[i] = geometrycollection_ogr_to_sfs(subgeom);
                break;
            default:
                Rcpp::stop("Unknown subgeometry type.");
        }
    }

    Rcpp::S4 sfs("sfs_geometrycollection");
    sfs.slot("geoms") = geoms;
    sfs.slot("data")  = Rcpp::DataFrame();
    sfs.slot("crs")   = CRS();

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_geometrycollection()
{
    OGRGeometryH geom;
    char* wkt = "GEOMETRYCOLLECTION(POINT(4 6), LINESTRING(4 6,7 10), POLYGON((0 0,0 1,1 0,0 0)))";

    OGRErr err = OGR_G_CreateFromWkt(&wkt, NULL, &geom);

    assert(err == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = geometrycollection_ogr_to_sfs(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}

