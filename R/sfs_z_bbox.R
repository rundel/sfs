setClassUnion("sfs_single_geom", c("sfs_point", "sfs_linestring", "sfs_polygon"))
setClassUnion("sfs_multiple_geom", c("sfs_multipoint", "sfs_multilinestring", "sfs_multipolygon",
                                     "sfs_geometrycollection", "sfs_featurecollection"))

setGeneric("bbox", function(x) standardGeneric("bbox"))

setMethod(
    "bbox",
    signature(x = "sfs_single_geom"),
    function(x)
    {
        apply(x@coords,2,range)
    }
)

setMethod(
    "bbox",
    signature(x = "sfs_multiple_geom"),
    function(x)
    {
        apply(
            do.call(rbind, lapply(x@geoms, bbox)),
            2, range
        )
    }
)
