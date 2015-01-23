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