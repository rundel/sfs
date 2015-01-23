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