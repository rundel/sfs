setClass("sfs_multipoint",
    representation(geoms = "list", crs = "CRS"),
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

multipoint = function(geoms, crs = CRS(""))
{
    # FIXME

    new("sfs_multipoint", geoms = geoms, crs = crs)
}