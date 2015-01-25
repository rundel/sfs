setClass("sfs_multipolygon",
    representation(geoms = "list", crs = "CRS"),
    validity = function(object)
    {
        if (any(sapply(object@geoms, function(x) class(x)) != "sfs_polygon"))
            return("all subgeometries must be of class sfs_polygon")

        # Validate Polygons
        lapply(object@geoms, function(x) validObject(x))

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)


validate_multigeom_crs = function(crs, sub_crs)
{
    stopifnot(all(!is.na(sub_crs)))

    # Handle CRS disagreements
    if (length(sub_crs) > 1)
        stop("subgeometries' CRS strings differ")

    if (is.na(crs@projargs))
    {
        crs = CRS("")
        if (length(sub_crs) == 1)
            crs = CRS(sub_crs)
    } else {
        if (length(sub_crs) == 1 & crs@projargs != polys_CRS)
            stop("Given CRS string does not match subgeometries' CRS strings")
    }

    return(crs)
}


multipolygon = function(polygons, crs = CRS(""), show_warnings = TRUE)
{
    stopifnot(typeof(polygons) == "list")
    stopifnot(all(sapply(polygons, function(x) class(x)) == "sfs_polygon"))

    polys_crs = sapply(polygons, function(x) x@CRS@projargs)
    polys_crs = polys_crs[!is.na(polys_crs)]
    polys_crs = unique(polys_crs)

    crs  = validate_multigeom_crs(crs, polys_crs)

    # Strip redundant polygon data
    polygons = lapply(polygons,
                      function(x)
                      {
                        x@crs = CRS("")
                        return(x)
                      }
                     )

    new("sfs_multipolygon", geoms = polygons, crs = crs)
}

plot_sfs_multipolygon = function(x, add=FALSE, xlab="", ylab="", ...)
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
    signature(x = "sfs_multipolygon", y = "missing"),
    function(x, y, ...)
        plot_sfs_multipolygon(x, ...)
)


