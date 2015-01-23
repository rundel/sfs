library(Rcpp)

registerPlugin("gdal",
                function() {
                    list(env = list(PKG_LIBS     = system("gdal-config --libs", intern=TRUE),
                                    PKG_CXXFLAGS = paste(system("gdal-config --cflags", intern=TRUE),
                                                         paste0("-I",path.expand(file.path(getwd(),"src"))))
                                   )
                    )
                }
              )

sourceCpp("src/gdal_drivers.cpp", rebuild=TRUE)
sourceCpp("src/ogr_util.cpp", rebuild=TRUE)
sourceCpp("src/ogr_to_R.cpp", rebuild=TRUE)

ogr_register_all()
gdal_register_all()


source("R/sfs_featurecollection.R")
source("R/sfs_geometrycollection.R")
source("R/sfs_linestring.R")
source("R/sfs_multilinestring.R")
source("R/sfs_multipoint.R")
source("R/sfs_multipolygon.R")
source("R/sfs_point.R")
source("R/sfs_polygon.R")
