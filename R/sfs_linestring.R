setClass("sfs_linestring",
    representation(coords = "matrix", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

linestring = function(coords, crs = CRS(""))
{
    # FIXME

    new("sfs_linestring", coords = coords, crs = crs)
}
