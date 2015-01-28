//[[Rcpp::export]]
Rcpp::S4 test_point()
{
    OGRGeometryH geom;
    char* wkt = "POINT (30 10)";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = point_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_multipoint()
{
    OGRGeometryH geom;
    char* wkt = "MULTIPOINT ((10 40), (40 30), (20 20), (30 10))";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = multipoint_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_linestring()
{
    OGRGeometryH geom;
    char* wkt = "LINESTRING (30 10, 10 30, 40 40)";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = linestring_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_multilinestring()
{
    OGRGeometryH geom;
    char* wkt = "MULTILINESTRING ((10 10, 20 20, 10 40),(40 40, 30 30, 40 20, 30 10))";

    assert(OGR_G_CreateFromWkt(&wkt, NULL, &geom) == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = multilinestring_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

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

    Rcpp::S4 sfs = polygon_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

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

    Rcpp::S4 sfs = multipolygon_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}

//[[Rcpp::export]]
Rcpp::S4 test_geometrycollection()
{
    OGRGeometryH geom;
    char* wkt = "GEOMETRYCOLLECTION(POINT(4 6), LINESTRING(4 6,7 10), POLYGON((0 0,0 1,1 0,0 0)))";

    OGRErr err = OGR_G_CreateFromWkt(&wkt, NULL, &geom);

    assert(err == OGRERR_NONE && geom != NULL);

    Rcpp::S4 sfs = geometrycollection_from_ogr(geom);
    OGR_G_DestroyGeometry(geom);

    return(sfs);
}