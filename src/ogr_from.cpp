// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>

#include "ogr_from.hpp"
#include "ogr_util.hpp"

///
/// Util
///

Rcpp::NumericMatrix get_coords(OGRGeometryH geom)
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
//// General Translate
////

Rcpp::S4 from_ogr(OGRGeometryH geom)
{
    Rcpp::S4 res;
    switch(OGR_G_GetGeometryType(geom))
    {
        case wkbPoint:
            res = point_from_ogr(geom);
            break;
        case wkbLineString:
            res = linestring_from_ogr(geom);
            break;
        case wkbPolygon:
            res = polygon_from_ogr(geom);
            break;
        case wkbMultiPoint:
            res = multipoint_from_ogr(geom);
            break;
        case wkbMultiLineString:
            res = multilinestring_from_ogr(geom);
            break;
        case wkbMultiPolygon:
            res = multipolygon_from_ogr(geom);
            break;
        case wkbGeometryCollection:
            res = geometrycollection_from_ogr(geom);
            break;
        default:
            Rcpp::stop("Unknown subgeometry type.");
    }

    return(res);
}


////
//// Point
////

Rcpp::S4 point_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbPoint, "Geometry must be a point");
    assert(OGR_G_GetGeometryCount(geom) == 0, "Subgeometries not allowed");
    assert(OGR_G_GetPointCount(geom) == 1, "Point should only have 1 coordinate");

    Rcpp::NumericMatrix coords = get_coords(geom);

    Rcpp::S4 sfs("sfs_point");
    sfs.slot("coords") = coords;
    sfs.slot("crs")    = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Multipoint
////

Rcpp::S4 multipoint_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiPoint, "Geometry must be a multipoint");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = point_from_ogr( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multipoint");
    sfs.slot("geoms") = geoms;
    sfs.slot("crs")   = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Linestring
////

Rcpp::S4 linestring_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbLineString, "Geometry must be a Linestring");
    assert(OGR_G_GetGeometryCount(geom) == 0, "Subgeometries not allowed");

    Rcpp::NumericMatrix coords = get_coords(geom);

    Rcpp::S4 sfs("sfs_linestring");
    sfs.slot("coords") = coords;
    sfs.slot("crs")    = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Multilinestring
////

Rcpp::S4 multilinestring_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiLineString, "Geometry must be a multilinestring");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = linestring_from_ogr( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multilinestring");
    sfs.slot("geoms") = geoms;
    sfs.slot("crs")   = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Polygon
////

Rcpp::S4 polygon_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbPolygon, "Geometry must be a Polygon");
    assert(OGR_G_GetGeometryCount(geom) >= 1, "There must be at least 1 subgeometry");

    OGR_G_CloseRings(geom);

    OGRGeometryH shell = OGR_G_GetGeometryRef(geom, 0);
    assert(OGR_G_GetGeometryType(shell) == wkbLineString, "Polygon shell should be a linestring");

    Rcpp::NumericMatrix coords = get_coords(shell);

    if(!is_cw(coords))
        coords = rev_mat(coords);

    Rcpp::List holes(OGR_G_GetGeometryCount(geom)-1);
    for(int i=1; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        OGRGeometryH hole = OGR_G_GetGeometryRef(geom, i);
        assert(OGR_G_GetGeometryType(hole) == wkbLineString, "Polygon holes should be a linestrings");

        Rcpp::NumericMatrix hcrd = get_coords(hole);

        if(is_cw(hcrd))
            hcrd = rev_mat(hcrd);

        holes[i-1] = hcrd;
    }


    Rcpp::S4 sfs("sfs_polygon");
    sfs.slot("coords") = coords;
    sfs.slot("holes")  = holes;
    sfs.slot("crs")    = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Multipolygon
////

Rcpp::S4 multipolygon_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbMultiPolygon, "Geometry must be a multipolygon");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = polygon_from_ogr( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_multipolygon");
    sfs.slot("geoms") = geoms;
    sfs.slot("crs")   = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


////
//// Geometry Collection
////

Rcpp::S4 geometrycollection_from_ogr(OGRGeometryH geom)
{
    assert(OGR_G_GetGeometryType(geom) == wkbGeometryCollection, "Geometry must be a geometry collection");

    Rcpp::List geoms(OGR_G_GetGeometryCount(geom));
    for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
    {
        geoms[i] = from_ogr( OGR_G_GetGeometryRef(geom, i) );
    }

    Rcpp::S4 sfs("sfs_geometrycollection");
    sfs.slot("geoms") = geoms;
    sfs.slot("crs")   = get_crs(OGR_G_GetSpatialReference(geom));

    return(sfs);
}


