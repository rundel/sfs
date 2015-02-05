// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>

#include "ogr_util.hpp"

// [[Rcpp::export]]
double ring_signed_area(Rcpp::NumericMatrix m)
{
    // Based on area formula given by
    // http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon

    assert(m(0,0) == m(m.nrow()-1,0) && m(0,1) == m(m.nrow()-1,1), "Ring must be closed.");

    double area = 0;
    for(int i=0; i < m.nrow()-1; ++i)
    {
        area += m(i,0)*m(i+1,1) -  m(i+1,0)*m(i,1);
    }

    return( area / 2. );
}

// [[Rcpp::export]]
bool is_cw(Rcpp::NumericMatrix m)
{
    return(ring_signed_area(m) < 0);
}

Rcpp::NumericMatrix rev_mat(Rcpp::NumericMatrix m)
{
    Rcpp::NumericMatrix n(m.nrow(),m.ncol());

    for(int i=0; i < m.nrow(); ++i)
    {
        n(m.nrow()-1-i, 0) = m(i, 0);
        n(m.nrow()-1-i, 1) = m(i, 1);
    }

    return(n);
}

Rcpp::S4 get_crs(OGRSpatialReferenceH sr)
{
    Rcpp::CharacterVector projargs(1);

    if (sr != NULL)
    {
        char* s = NULL;
        OSRExportToProj4(sr, &s);

        if(s != NULL)
        {
            projargs[0] = std::string(s);
            OGRFree(s);
        }
        else
        {
            projargs[0] = NA_STRING;
        }
    }
    else
    {
        projargs[0] = NA_STRING;
    }

    Rcpp::S4 crs("CRS");
    crs.slot("projargs") = projargs;

    return(crs);
}


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
    OGRSFDriverH *driver = NULL;
    DataSource data( OGROpen(file.c_str(), false, driver) );

    if(data.get() == NULL) Rcpp::stop("Unable to open file.");

    Rcpp::Rcout << "File  : " << OGR_DS_GetName( data.get() ) << "\n"
                << "Driver: " << OGR_Dr_GetName( OGR_DS_GetDriver(data.get()) ) << "\n";

    for(int i=0; i < OGR_DS_GetLayerCount(data.get()); ++i)
    {
        OGRLayerH layer = OGR_DS_GetLayer(data.get(), i);
        if(layer == NULL) Rcpp::stop("Unable to get layer.");

        Rcpp::Rcout << "==========================================================================================\n";
        Rcpp::Rcout << "Layer " << i << " : " << OGR_L_GetName(layer) << "\n";
        Rcpp::Rcout << "Type : " << OGRGeometryTypeToName( OGR_L_GetGeomType(layer) ) << "\n";

        for(int j=0; j < OGR_L_GetFeatureCount(layer, false); ++j)
        {
            Feature feat( OGR_L_GetNextFeature(layer) );
            OGR_F_DumpReadable(feat.get(), NULL);
        }
    }
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
void dump2_ogr(std::string file = "test.geojson")
{
    OGRSFDriverH *driver = NULL;
    DataSource data( OGROpen(file.c_str(), false, driver) );

    if(data.get() == NULL) Rcpp::stop("Unable to open file.");

    Rcpp::Rcout << "File  : " << OGR_DS_GetName(data.get()) << "\n"
                << "Driver: " << OGR_Dr_GetName( OGR_DS_GetDriver(data.get()) ) << "\n";

    for(int i=0; i < OGR_DS_GetLayerCount(data.get()); ++i)
    {
        OGRLayerH layer = OGR_DS_GetLayer(data.get(), i);
        if(layer == NULL) Rcpp::stop("Unable to get layer.");

        Rcpp::Rcout << "Layer " << i << " : " << OGR_L_GetName(layer) << "\n";
        Rcpp::Rcout << "    Type : " << OGRGeometryTypeToName( OGR_L_GetGeomType(layer) ) << "\n";


        OGRSpatialReferenceH sr = OGR_L_GetSpatialRef(layer);
        if (sr != NULL)
        {
            char* wkt = NULL;
            OSRExportToProj4(sr, &wkt);

            if(wkt != NULL)
            {
                Rcpp::Rcout << "    proj4 : " << wkt << "\n";
                OGRFree(wkt);
            }
        }

        for(int j=0; j < OGR_L_GetFeatureCount(layer, false); ++j)
        {
            Feature feat( OGR_L_GetNextFeature(layer) );

            Rcpp::Rcout << "    Feature " << j << " : "
                        << OGR_F_GetFieldCount(feat.get()) << " fields\n";

            if (OGR_F_GetFieldCount(feat.get()) > 0)
            {
                for(int k=0; k < OGR_F_GetFieldCount(feat.get()); ++k)
                {
                    OGRFieldDefnH field_def = OGR_F_GetFieldDefnRef(feat.get(), k);
                    if (field_def == NULL) Rcpp::stop("Unable to get field.");

                    Rcpp::Rcout << "        Field " << k << " : "
                                << OGR_Fld_GetNameRef(field_def) << " ("
                                << OGR_GetFieldTypeName(OGR_Fld_GetType(field_def)) << ") = "
                                << OGR_F_GetFieldAsString (feat.get(), k)
                                << "\n";
                }
            }

            // For the time being assume only one geometry per feature
            // capatable with GDAL <1.11

            OGRGeometryH geom = OGR_F_GetGeometryRef(feat.get());

            if (geom == NULL) Rcpp::stop("Unable to get geometry.");

            std::vector<int> idx;
            idx.push_back(0);

            process_geom(idx, geom);
        }
    }
}
