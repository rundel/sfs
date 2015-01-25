setClass("sfs_geometrycollection",
    representation(geoms = "list", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

geometry_collection = function(geoms, crs = CRS(""))
{
    # FIXME

    new("sfs_geometrycollection", geoms = geoms, crs = crs)
}

plot_sfs_geometrycollection = function(x, add=FALSE, col="black", xlab="", ylab="", ...)
{
    bound = bbox(x)

    if (!add)
        plot(x=range(bound[,1]), y=range(bound[,2]), xlab=xlab, ylab=ylab, type="n", ...)

    col = rep(col, length.out=length(x@geoms))

    for(i in seq_along(x@geoms))
    {
        plot(x@geoms[[i]], add=TRUE, col=col[i], ...)
    }
}

setMethod(
    "plot",
    signature(x = "sfs_geometrycollection", y = "missing"),
    function(x, y, ...)
        plot_sfs_geometrycollection(x, ...)
)


