setClass("sfs_featurecollection",
    representation(geoms = "list", data = "data.frame", crs = "CRS"),
    validity = function(object)
    {
        # FIXME

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)

feature_collection = function(geoms, data = data.frame(), crs = CRS(""))
{
    # FIXME

    new("sfs_featurecollection", geoms = geoms, data = data, crs = crs)
}

plot_sfs_featurecollection = function(x, add=FALSE, col="black", xlab="", ylab="", ...)
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
    signature(x = "sfs_featurecollection", y = "missing"),
    function(x, y, ...)
        plot_sfs_featurecollection(x, ...)
)


