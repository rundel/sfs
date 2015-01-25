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

plot_sfs_multilinestring = function(x, add=FALSE, xlab="", ylab="", ...)
{
    bound = bbox(x)

    if (!add)
        plot(x=range(bound[,1]), y=range(bound[,2]), xlab=xlab, ylab=ylab, type="n", ...)

    for(g in x@geoms)
    {
        plot(g, add=TRUE, ...)
    }
}

setMethod(
    "plot",
    signature(x = "sfs_multilinestring", y = "missing"),
    function(x, y, ...)
        plot_sfs_multilinestring(x, ...)
)


