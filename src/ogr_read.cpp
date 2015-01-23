// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>

#include "ogr_to_R.hpp"
#include "ogr_raii.hpp"

Rcpp::DataFrame feature_to_df(OGRFeatureH feat)
{
    int fields = OGR_F_GetFieldCount(feat);
    Rcpp::DataFrame data(fields);

    if (fields > 0)
    {
        Rcpp::CharacterVector names(fields);

        for(int k=0; k < fields; ++k)
        {
            OGRFieldDefnH field_def = OGR_F_GetFieldDefnRef(feat, k);

            names[k] = OGR_Fld_GetNameRef(field_def);

            switch(OGR_Fld_GetType(field_def))
            {
                case OFTInteger:
                    Rcpp::IntegerVector col(1);
                    col[0] = OGR_F_GetFieldAsInteger(feat, k);
                    data(k) = col;
                    break;
                case OFTReal:
                    Rcpp::NumericVector col(1);
                    col[0] = OGR_F_GetFieldAsDouble(feat, k);
                    data(k) = col;
                    break;
                case OFTString:
                case OFTIntegerList:
                case OFTRealList:
                case OFTStringList:
                case OFTDate:
                case OFTTime:
                case OFTDateTime:
                    Rcpp::CharacterVector col(1);
                    col[0] = std::string(OGR_F_GetFieldAsString(feat, k));
                    data(k) = col;
                    break;
                case OFTWideString:
                case OFTWideStringList:
                case OFTBinary:
                    Rcpp::CharacterVector col(1);
                    col[0] = "Unsupported type";
                    data(k) = col;
                    break;
            }
        }

        data.attr("names") = names;
    }

    return(data)
}

// [[Rcpp::export]]
Rcpp::S4 read_ogr(std::string file, int layer_index = 0)
{
    OGRSFDriverH *driver;
    DataSource data( OGROpen(file.c_str(), false, driver) );

    assert(data.get() == NULL, "Unable to open file.");
    assert(layer_index >= OGR_DS_GetLayerCount(data) || layer_index < 0, "Invalid layer index.");

    OGRLayerH layer = OGR_DS_GetLayer(data.get(), layer_index);
    assert(layer == NULL, "Unable to retrieve layer.");

    // Get CRS string (proj4 format)
    std::string crs = "";
    
    if (OGR_L_GetSpatialRef(layer) != NULL)
    {
        char* tmp = NULL;
        OSRExportToProj4(OGR_L_GetSpatialRef(layer), &wkt);

        if(tmp != NULL)
        {
            crs = std::string(tmp);
            OGRFree(wkt);
        }
    }

    OGRwkbGeometryType type = OGR_L_GetGeomType(layer)
    assert(type != NULL, "Unable to retrieve layer type.")

    if (type == wkbPoint      || type == wkbLineString      || type == wkbPolygon      ||
        type == wkbMultiPoint || type == wkbMultiLineString || type == wkbMultiPolygon ||
        type == wkbGeometryCollection)
    {
        assert(OGR_L_GetFeatureCount(layer, false) > 1, "Too many features for type.");

        OGRFeatureH feat = OGR_L_GetNextFeature(layer);
        assert(feat == NULL, "Unable to retrieve feature.");


        Rcpp::S4 geom = ogr_to_sfs(OGR_F_GetGeomRef(feat));
        if (OGR_F_GetFieldCount(feat) == 0) // Basic geometry type - no data
        {

        }
        else // Feature type - has data
        {

        }

        OGR_F_Destroy(feat);
    }
    else if (type == wkbUnknown) // Assuming Feature Collection
    {

        for(int j=0; j < OGR_L_GetFeatureCount(layer, false); ++j)
        {
            OGRFeatureH feat = OGR_L_GetNextFeature(layer);
            if(feat == NULL)
            {
                OGRReleaseDataSource(data);
                Rcpp::stop("Unable to retrieve feature.");
            }

            

            if (OGR_F_GetGeomFieldCount(feat) != 0)
            {

                Rcpp::stop("Unsupported number of geometries")
            }
                for(int k=0; k < OGR_F_GetGeomFieldCount(feat); ++k)
                {
                    OGRGeometryH geom = OGR_F_GetGeomFieldRef(feat, k);
                    if (geom == NULL) Rcpp::stop("Unable to get geometry.");

                    std::vector<int> idx;
                    idx.push_back(k);

                    process_geom(idx, geom);

                    //Rcpp::Rcout << "        Geometry " << k << " : "
                    //            << OGRGeometryTypeToName(OGR_G_GetGeometryType(geom)) << " ("
                    //            << OGR_G_GetGeometryCount(geom) << ") "
                    //            << OGR_G_GetDimension(geom) << ", "
                    //            << OGR_G_GetCoordinateDimension(geom)
                    //            << "\n";
                }
            }


            OGR_F_Destroy(feat);
        }
    }

    OGRReleaseDataSource(data);
}
