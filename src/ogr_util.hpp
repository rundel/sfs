#ifndef OGR_UTIL_HPP
#define OGR_UTIL_HPP

#include <Rcpp.h>
#include <ogr_api.h>

double ring_signed_area(Rcpp::NumericMatrix m);
bool is_cw(Rcpp::NumericMatrix m);
Rcpp::NumericMatrix rev_mat(Rcpp::NumericMatrix m);

Rcpp::S4 get_crs(OGRSpatialReferenceH sr);

inline void assert(bool test, const std::string& error = "Assertion Failed.")
{
    if(!test) Rcpp::stop(error);
}

class DataSource
{
    public:
        explicit DataSource(OGRDataSourceH data)
        : ds(data) {}

        ~DataSource()
        {
            if (ds != NULL)
                OGRReleaseDataSource(ds);
        }

        OGRDataSourceH get()
        {
            return ds;
        }
    private:
        OGRDataSourceH ds;
};

class Feature
{
    public:
        explicit Feature(OGRFeatureH feat)
        : f(feat) {}

        ~Feature()
        {
            if (f != NULL)
                OGR_F_Destroy(f);
        }

        OGRFeatureH get()
        {
            return f;
        }
    private:
        OGRFeatureH f;
};

#endif
