setClass("sfs_point",
    representation(coords = "matrix",
                   data = "data.frame",
                   crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate Data
        if (!(nrow(object@data) %in% 0:1))
            return("data may contain at most one row")

        validObject(object@crs)

        return(TRUE)
    }
)

point = function(coords,  data = data.frame(), crs = CRS(""))
{
    # FIXME

    new("sfs_point", coords = coords, data = data, crs = crs)
}
