// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>

#include "ogr_from.hpp"
#include "ogr_util.hpp"


Rcpp::DataFrame init_data(OGRFeatureH feat, int n_features)
{
    int fields = OGR_F_GetFieldCount(feat);
    Rcpp::DataFrame data;

    if (fields > 0)
    {
        Rcpp::CharacterVector names(fields);

        for(int i=0; i < fields; ++i)
        {
            OGRFieldDefnH field_def = OGR_F_GetFieldDefnRef(feat, i);
            assert(field_def != NULL, "Unable to retrieve field definition");

            names[i] = OGR_Fld_GetNameRef(field_def);

            OGRFieldType type = OGR_Fld_GetType(field_def);
            switch(type)
            {
                case OFTInteger:
                {
                    data.push_back(Rcpp::IntegerVector(n_features));
                    break;
                }
                case OFTReal:
                {
                    data.push_back(Rcpp::NumericVector(n_features));
                    break;
                }
                default:
                {
                    data.push_back(Rcpp::CharacterVector(n_features));
                    break;
                }
            }
        }

        data.attr("names") = names;
    }

    return(data);
}

void fill_data(Rcpp::DataFrame& data, OGRFeatureH feat, int row)
{
    int fields = OGR_F_GetFieldCount(feat);

    assert(fields == data.length(), "Field lengths do not match");

    for(int i=0; i < fields; ++i)
    {
        OGRFieldDefnH field_def = OGR_F_GetFieldDefnRef(feat, i);
        assert(field_def != NULL, "Unable to retrieve field definition");

        std::string name( OGR_Fld_GetNameRef(field_def) );
        int col = data.findName(name);

        OGRFieldType type = OGR_Fld_GetType(field_def);
        switch(type)
        {
            case OFTInteger:
            {
                Rcpp::IntegerVector vals(data[col]);
                vals[row] = OGR_F_GetFieldAsInteger(feat, i);
                data[col] = vals;
                break;
            }
            case OFTReal:
            {
                Rcpp::NumericVector vals(data[col]);
                vals[row] = OGR_F_GetFieldAsDouble(feat, i);
                data[col] = vals;
                break;
            }
            case OFTString:
            case OFTIntegerList:
            case OFTRealList:
            case OFTStringList:
            case OFTDate:
            case OFTTime:
            case OFTDateTime:
            {
                Rcpp::CharacterVector vals(data[col]);
                vals[row] = std::string(OGR_F_GetFieldAsString(feat, i));
                data[col] = vals;
                break;
            }
            default:
            {
                Rcpp::Rcout << "Warning: Unsupported type (row: " << row << ", col: " << col << ")\n";
                Rcpp::CharacterVector vals(data[col]);
                vals[row] = Rcpp::CharacterVector::get_na();
                data[col] = vals;
                break;
            }
        }
    }
}


// [[Rcpp::export]]
Rcpp::S4 read_ogr(std::string file, int layer_index = 0)
{
    OGRSFDriverH *driver = NULL;
    DataSource data( OGROpen(file.c_str(), false, driver) );

    assert(data.get() != NULL, "Unable to open file.");

    assert(layer_index < OGR_DS_GetLayerCount(data.get()) || layer_index >= 0, "Invalid layer index.");

    OGRLayerH layer = OGR_DS_GetLayer(data.get(), layer_index);
    assert(layer != NULL, "Unable to retrieve layer.");

    // Get CRS string (proj4 format)
    Rcpp::S4 crs = get_crs( OGR_L_GetSpatialRef(layer) );

    Rcpp::S4 res;

    OGRwkbGeometryType type = OGR_L_GetGeomType(layer);
    if (   type == wkbPoint      || type == wkbLineString      || type == wkbPolygon
        || type == wkbMultiPoint || type == wkbMultiLineString || type == wkbMultiPolygon
        || type == wkbGeometryCollection || type == wkbUnknown)
    {
        int n_feat = OGR_L_GetFeatureCount(layer, false);

        Rcpp::DataFrame data;
        Rcpp::List geoms(n_feat);

        for(int j=0; j < n_feat; ++j)
        {
            Feature feat(OGR_L_GetNextFeature(layer));
            assert(feat.get() != NULL, "Unable to retrieve feature.");

            if (j==0)
                data = init_data(feat.get(), n_feat);

            fill_data(data, feat.get(), j);
            geoms[j] = from_ogr( OGR_F_GetGeometryRef(feat.get()) );
        }

        if (n_feat == 1 && data.length() == 0)
        {
            res = Rcpp::S4(geoms[0]);
            res.slot("crs") = crs;
        }
        else // Feature type - has data
        {
            res = Rcpp::S4("sfs_featurecollection");

            res.slot("geoms") = geoms;
            res.slot("data")  = data;
            res.slot("crs")   = crs;
        }
    }
    else
    {
        Rcpp::stop("Unsupported layer type");
    }

    return(res);
}
