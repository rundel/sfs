// [[Rcpp::plugins(gdal)]]

#include <Rcpp.h>
#include <gdal.h>

// [[Rcpp::export]]
void gdal_register_all()
{
    GDALAllRegister();
}

// [[Rcpp::export]]
Rcpp::DataFrame get_gdal_drivers()
{
    int n = GDALGetDriverCount();
    //Rcpp::Rcout << "Number of GDAL drivers: " << n << "\n";

    Rcpp::CharacterVector short_name(n);
    Rcpp::CharacterVector long_name(n);

    for(int i=0; i<n; ++i)
    {
        GDALDriverH d = GDALGetDriver(i);
        short_name[i]  = std::string(GDALGetDriverShortName(d));
        long_name[i]   = std::string(GDALGetDriverLongName(d));

        //char** strs = GDALGetMetadata(d, NULL);
        //
        //if (i < 3)
        //{
        //    int j = 0;
        //    while(true)
        //    {
        //        if (strs[j] == NULL)
        //            break;
        //        Rcpp::Rcout << j << ": " << strs[j] << "\n";
        //        j++;
        //    }
        //}
        //
        //Rcpp::Rcout << "\n";
    }

    return(
        Rcpp::DataFrame::create(Rcpp::Named("short") = short_name,
                                Rcpp::Named("long") = long_name)
    );
}
