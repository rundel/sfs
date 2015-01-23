library(Rcpp)

registerPlugin("gdal",
                function() {
                    list(env = list(PKG_LIBS=paste0("-lgdal")
                                    #PKG_CXXFLAGS=paste0("-I",path.expand("~/local/include"))
                                   ))
                }
              )

sourceCpp("ogr_polygon.cpp", rebuild=TRUE)
sourceCpp("gdal_drivers.cpp", rebuild=TRUE)
sourceCpp("ogr_to_R.cpp", rebuild=TRUE)

ogr_register_all()
gdal_register_all()