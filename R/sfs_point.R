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

plot_sfs_point = function(x, add=FALSE, xlab="", ylab="", ...)
{
    bound = bbox(x)

    if (!add)
        plot(x=range(bound[,1]), y=range(bound[,2]), xlab=xlab,ylab=ylab, type="n", ...)

    points(x@coords, ...)
}

setMethod(
    "plot",
    signature(x = "sfs_point", y = "missing"),
    function(x, y, ...) 
        plot_sfs_point(x, ...)
)

