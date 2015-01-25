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

plot_sfs_linestring = function(x, add=FALSE, xlab="", ylab="", ...)
{
    bound = bbox(x)

    if (!add)
        plot(x=range(bound[,1]), y=range(bound[,2]), xlab=xlab,ylab=ylab, type="n", ...)

    lines(x@coords, ...)
}

setMethod(
    "plot",
    signature(x = "sfs_linestring", y = "missing"),
    function(x, y, ...) 
        plot_sfs_linestring(x, ...)
)

