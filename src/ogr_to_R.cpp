// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>

#include "ogr_to_R.hpp"
#include "ogr_raii.hpp"

///
/// Util
///

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

Rcpp::S4 CRS(std::string s = "")
{
    Rcpp::S4 crs("CRS");
    Rcpp::CharacterVector projargs = s.empty() ? NA_STRING : Rcpp::wrap(s);
    crs.slot("projargs") = projargs;

    return(crs);
}

////
//// General Translate
////

Rcpp::S4 ogr_to_sfs(OGRGeometryH geom)
{
    Rcpp::S4 res;
    switch(OGR_G_GetGeometryType(geom))
    {
        case wkbPoint:
            res = point_ogr_to_sfs(geom);
            break;
        case wkbLineString:
            res = linestring_ogr_to_sfs(geom);
            break;
        case wkbPolygon:
            res = polygon_ogr_to_sfs(geom);
            break;
        case wkbMultiPoint:
            res = multipoint_ogr_to_sfs(geom);
            break;
        case wkbMultiLineString:
            res = multilinestring_ogr_to_sfs(geom);
            break;
        case wkbMultiPolygon:
            res = multipolygon_ogr_to_sfs(geom);
            break;
        case wkbGeometryCollection:
            res = geometrycollection_ogr_to_sfs(geom);
            break;
        default:
            Rcpp::stop("Unknown subgeometry type.");
    }

    return(res);
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
    sfs.slot("crs")    = CRS();

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
    sfs.slot("crs")   = CRS();

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
    sfs.slot("crs")    = CRS();

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
    sfs.slot("crs")   = CRS();

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
    sfs.slot("crs")    = CRS();

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
    sfs.slot("crs")   = CRS();

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
        geoms[i] = ogr_to_sfs( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_geometrycollection");
    sfs.slot("geoms") = geoms;
    sfs.slot("crs")   = CRS();

    return(sfs);
}


