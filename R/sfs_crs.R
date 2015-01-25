setClass("CRS",
    representation(projargs = "character"),
    validity = function(object) {
        if (length(object@projargs) != 1)
          return("projargs must be of length 1")

        return(TRUE)
    }
)
