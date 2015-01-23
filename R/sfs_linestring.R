setClass("sfs_linestring",
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

linestring = function(coords,  data = data.frame(), crs = CRS(""))
{
    # FIXME

    new("sfs_linestring", coords = coords, data = data, crs = crs)
}
