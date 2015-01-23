setClass("sfs_geometrycollection",
    representation(geoms = "list",
                   data = "data.frame",
                   crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate Sub-Geometries
        lapply(object@geoms, function(x) validObject(x))

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

geometry_collection = function(geoms, data = data.frame(), crs = CRS(""))
{
    # FIXME

    new("sfs_geometrycollection",
        geoms = geoms, data = data, crs = crs)
}