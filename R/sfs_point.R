setClass("sfs_point",
    representation(coords = "matrix", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

point = function(coords,  data = data.frame(), crs = CRS(""))
{
    # FIXME

    new("sfs_point", coords = coords, data = data, crs = crs)
}
