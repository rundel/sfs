#ifndef OGR_TO_R_HPP
#define OGR_TO_R_HPP

#include <Rcpp.h>
#include <ogr_api.h>

Rcpp::S4 ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 point_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 multipoint_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 linestring_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 multilinestring_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 polygon_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 multipolygon_ogr_to_sfs(OGRGeometryH geom);
Rcpp::S4 geometrycollection_ogr_to_sfs(OGRGeometryH geom);

#endif