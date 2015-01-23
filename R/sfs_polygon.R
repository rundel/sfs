polygon_signed_area = function(xy)
{
    x = xy[,1]
    y = xy[,2]
    n = nrow(xy)

    stopifnot(x[1] == x[n] & y[1] == y[n])

    # Based on area formula given by
    # http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon

    return( sum(x[1:(n-1)]*y[2:n]-x[2:n]*y[1:(n-1)])/2 )
}

is_cw = function(xy)
{
    return(polygon_signed_area(xy) < 0)
}

setClass("sfs_polygon",
    representation(coords = "matrix", holes = "list", crs = "CRS"),
    validity = function(object)
    {
        # Validate coords Ring
        if (ncol(object@coords) != 2)
            return("coords ring must have only 2 dimensions")
        if (nrow(object@coords) < 4)
            return("coords ring must have at least 4 coordinates")
        if (!all(object@coords[1,] == object@coords[nrow(object@coords),]))
            return("coords ring must start and end at same coordinate")
        if (any(is.na(object@coords)))
            return("coords ring must not have any missing coordinates")
        if (!is_cw(object@coords))
            return("coords ring should be clockwise")

        # Validate Holes
        if (length(object@holes) != 0)
        {
            hole_details = do.call(rbind,
                                   lapply(object@holes,
                                          function(x) data.frame(class = class(x),
                                                                 type  = typeof(x),
                                                                 ncol  = ncol(x),
                                                                 nrow  = nrow(x),
                                                                 isna  = any(is.na(x)),
                                                                 iscw  = is_cw(x)
                                                                )
                                          )
                                   )

            if (any(hole_details$class != "matrix"))
                return("all hole entries must be a matrix")
            if (any(hole_details$type != "double"))
                return("all holes must have coordinates of type double")
            if (any(hole_details$ncol != 2))
                return("all holes must have only 2 dimensions")
            if (any(hole_details$nrow < 4))
                return("all hole rings must have at least 4 coordinates")
            if (!all(sapply(object@holes, function(x) all(x[1,] == x[nrow(x),]))))
                return("all hole rings must start and end at same coordinate")
            if (any(hole_details$isna))
                return("all holes must not have any missing coordinates")
            if (any(hole_details$iscw))
                return("all holes must be counter clockwise")
        }

        validObject(object@crs)

        return(TRUE)
    }
)

polygon = function(coords, holes = list(), crs = CRS(""))
{
    # coords ring should be clockwise
    if (!is_cw(coords))
    {
        coords = coords[nrow(coords):1,]
    }

    # Hole boundaries should be counter clockwise
    holes = lapply(holes, function(x) {
                if (is_cw(x)) return(x[nrow(x):1,])
                else          return(x)
            })


    new("sfs_polygon", coords = coords, holes = holes, crs = crs)
}
