#ifndef OGR_RAII_HPP
#define OGR_RAII_HPP

#include <ogr_api.h>

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
