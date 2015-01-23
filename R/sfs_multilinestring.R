setClass("sfs_multilinestring",
    representation(geoms = "list", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

linestring = function(geoms, crs = CRS(""))
{
    # FIXME

    new("sfs_linestring", geoms = geoms, crs = crs)
}