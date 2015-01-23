setClass("sfs_multipolygon",
    representation(geoms = "list",
                   data = "data.frame",
                   crs = "CRS"),
    validity = function(object)
    {
        if (any(sapply(object@geoms, function(x) class(x)) != "sfs_polygon"))
            return("all subgeometries must be of class sfs_polygon")
        if (!(nrow(data) %in% 0:1))
            return("data.frame rows must be either 0 or 1")

        # Validate Polygons
        lapply(object@geoms, function(x) validObject(x))

        # Validate CRS
        validObject(object@crs)

        return(TRUE)
    }
)


validate_multigeom_data = function(data, sub_data)
{
    # Handle data disagreements
    if (nrow(sub_data) > 1)
    {
        sub_data = data.frame()
        if (show_warnings)
            warning("subgeometries' data are different from one another, ignoring")
    }

    if (nrow(data) == 0)
    {
        if (nrow(sub_data) == 1)
            data = sub_data
    } else {
        if (!identical(data, sub_data) & show_warnings) 
            warning("Given data does not match subgeometries' data, ignoring subgeometries' data")
    }

    return(data)
}

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


multipolygon = function(polygons, data = data.frame(), crs = CRS(""), show_warnings = TRUE)
{
    stopifnot(typeof(polygons) == "list")
    stopifnot(all(sapply(polygons, function(x) class(x)) == "sfs_polygon"))

    polys_data = unique( do.call(rbind, lapply(polygons, function(x) x@data)) )

    polys_crs = sapply(polygons, function(x) x@CRS@projargs)
    polys_crs = polys_crs[!is.na(polys_crs)]
    polys_crs = unique(polys_crs)

    data = validate_multigeom_data(data, polys_data)
    crs  = validate_multigeom_crs(crs, polys_crs)

    # Strip redundant polygon data
    polygons = lapply(polygons,
                      function(x)
                      {
                        x@data = data.frame()
                        x@crs = CRS("")
                        return(X)
                      }
                     )

    new("sfs_multipolygon",
        geoms = polygons,
        data = data, crs = crs)
}
