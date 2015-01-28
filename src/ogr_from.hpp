#ifndef OGR_FROM_HPP
#define OGR_FROM_HPP

#include <Rcpp.h>
#include <ogr_api.h>

Rcpp::S4 from_ogr(OGRGeometryH geom);
Rcpp::S4 point_from_ogr(OGRGeometryH geom);
Rcpp::S4 multipoint_from_ogr(OGRGeometryH geom);
Rcpp::S4 linestring_from_ogr(OGRGeometryH geom);
Rcpp::S4 multilinestring_from_ogr(OGRGeometryH geom);
Rcpp::S4 polygon_from_ogr(OGRGeometryH geom);
Rcpp::S4 multipolygon_from_ogr(OGRGeometryH geom);
Rcpp::S4 geometrycollection_from_ogr(OGRGeometryH geom);

#endif
