// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>

#include <ogr_api.h>

// [[Rcpp::export]]
void ogr_register_all()
{
    OGRRegisterAll();
}

// [[Rcpp::export]]
void ogr_cleanup_all()
{
    OGRCleanupAll();
}

// [[Rcpp::export]]
Rcpp::DataFrame ogr_drivers()
{
    int n = OGRGetDriverCount();
    //Rcpp::Rcout << "Number of OGR drivers: " << n << "\n";

    Rcpp::CharacterVector names(n);
    Rcpp::LogicalVector create(n);
    Rcpp::LogicalVector del(n);

    for(int i=0; i<n; ++i)
    {
        OGRSFDriverH d = OGRGetDriver(i);
        names[i]  = std::string(OGR_Dr_GetName(d));
        create[i] = OGR_Dr_TestCapability(d, ODrCCreateDataSource);
        del[i]    = OGR_Dr_TestCapability(d, ODrCDeleteDataSource);
    }

    return(
        Rcpp::DataFrame::create(Rcpp::Named("driver") = names,
                                Rcpp::Named("create") = create,
                                Rcpp::Named("delete") = del)
    );
}

void process_geom(std::vector<int> idx, OGRGeometryH geom)
{
    Rcpp::Rcout << "    ";
    for(int i=0; i < idx.size(); ++i)
        Rcpp::Rcout << "    ";

    Rcpp::Rcout << "Geometry "; 

    for(int i=0; i < idx.size(); ++i)
    {
        Rcpp::Rcout << idx[i];
        if (i != idx.size()-1)
            Rcpp::Rcout << ".";
    }


    Rcpp::Rcout << " : "
                << OGRGeometryTypeToName(OGR_G_GetGeometryType(geom)) << " ("
                << OGR_G_GetGeometryCount(geom) << ") "
                << OGR_G_GetDimension(geom) << ", "
                << OGR_G_GetCoordinateDimension(geom)
                << "\n";

    if (OGR_G_GetGeometryCount(geom) > 0)
    {
        for(int i=0; i < OGR_G_GetGeometryCount(geom); ++i)
        {
            std::vector<int> new_idx = idx;
            new_idx.push_back(i);
            process_geom(new_idx, OGR_G_GetGeometryRef(geom, i));
        }
    }
}

// [[Rcpp::export]]
void read_ogr(std::string file = "test.geojson")
{
    OGRSFDriverH *driver;
    OGRDataSourceH data = OGROpen(file.c_str(), false, driver);

    if(data == NULL)
        Rcpp::stop("Unable to open file.");

    Rcpp::Rcout << "File  : " << OGR_DS_GetName(data) << "\n"
                << "Driver: " << OGR_Dr_GetName( OGR_DS_GetDriver(data) ) << "\n";

    for(int i=0; i < OGR_DS_GetLayerCount(data); ++i)
    {
        OGRLayerH layer = OGR_DS_GetLayer(data, 0);
        Rcpp::Rcout << "Layer " << i << " : " << OGR_L_GetName(layer) << "\n";
        Rcpp::Rcout << "    Type : " << OGRGeometryTypeToName( OGR_L_GetGeomType(layer) ) << "\n";

        for(int j=0; j < OGR_L_GetFeatureCount(layer, false); ++j)
        {
            OGRFeatureH feat = OGR_L_GetNextFeature(layer);

            //OGR_F_DumpReadable(feat, NULL);

            Rcpp::Rcout << "    Feature " << j << " : "
                        << OGR_F_GetFieldCount(feat) << " fields, "
                        << OGR_F_GetGeomFieldCount(feat) << " geometry\n";

            if (OGR_F_GetFieldCount(feat) > 0)
            {
                for(int k=0; k < OGR_F_GetFieldCount(feat); ++k)
                {
                    OGRFieldDefnH field_def = OGR_F_GetFieldDefnRef(feat, k);
                    if (field_def == NULL) Rcpp::stop("Unable to get field.");

                    Rcpp::Rcout << "        Field " << k << " : "
                                << OGR_Fld_GetNameRef(field_def) << " ("
                                << OGR_GetFieldTypeName(OGR_Fld_GetType(field_def)) << ") = "
                                << OGR_F_GetFieldAsString (feat, k)
                                << "\n";
                }
            }

            if (OGR_F_GetGeomFieldCount(feat) > 0)
            {
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


// [[Rcpp::export]]
void dump_ogr(std::string file = "test.geojson")
{
    OGRSFDriverH *driver;
    OGRDataSourceH data = OGROpen(file.c_str(), false, driver);

    if(data == NULL)
        Rcpp::stop("Unable to open file.");

    Rcpp::Rcout << "File  : " << OGR_DS_GetName(data) << "\n"
                << "Driver: " << OGR_Dr_GetName( OGR_DS_GetDriver(data) ) << "\n";

    for(int i=0; i < OGR_DS_GetLayerCount(data); ++i)
    {
        OGRLayerH layer = OGR_DS_GetLayer(data, 0);
        Rcpp::Rcout << "==========================================================================================\n";
        Rcpp::Rcout << "Layer " << i << " : " << OGR_L_GetName(layer) << "\n";
        Rcpp::Rcout << "Type : " << OGRGeometryTypeToName( OGR_L_GetGeomType(layer) ) << "\n";

        for(int j=0; j < OGR_L_GetFeatureCount(layer, false); ++j)
        {
            OGRFeatureH feat = OGR_L_GetNextFeature(layer);

            OGR_F_DumpReadable(feat, NULL);

            OGR_F_Destroy(feat);
        }
    }

    OGRReleaseDataSource(data);
}