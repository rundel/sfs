setClass("sfs_geometrycollection",
    representation(geoms = "list", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

geometry_collection = function(geoms, crs = CRS(""))
{
    # FIXME

    new("sfs_geometrycollection", geoms = geoms, crs = crs)
}