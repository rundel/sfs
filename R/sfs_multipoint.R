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

plot_sfs_multipoint = function(x, add=FALSE, xlab="", ylab="", ...)
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
    signature(x = "sfs_multipoint", y = "missing"),
    function(x, y, ...)
        plot_sfs_multipoint(x, ...)
)


