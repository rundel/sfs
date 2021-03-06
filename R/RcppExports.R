# This file was generated by Rcpp::compileAttributes
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

gdal_register_all <- function() {
    invisible(.Call('sfs_gdal_register_all', PACKAGE = 'sfs'))
}

get_gdal_drivers <- function() {
    .Call('sfs_get_gdal_drivers', PACKAGE = 'sfs')
}

read_ogr <- function(file, layer_index = 0L) {
    .Call('sfs_read_ogr', PACKAGE = 'sfs', file, layer_index)
}

ring_signed_area <- function(m) {
    .Call('sfs_ring_signed_area', PACKAGE = 'sfs', m)
}

is_cw <- function(m) {
    .Call('sfs_is_cw', PACKAGE = 'sfs', m)
}

ogr_register_all <- function() {
    invisible(.Call('sfs_ogr_register_all', PACKAGE = 'sfs'))
}

ogr_cleanup_all <- function() {
    invisible(.Call('sfs_ogr_cleanup_all', PACKAGE = 'sfs'))
}

ogr_drivers <- function() {
    .Call('sfs_ogr_drivers', PACKAGE = 'sfs')
}

dump_ogr <- function(file = "test.geojson") {
    invisible(.Call('sfs_dump_ogr', PACKAGE = 'sfs', file))
}

dump2_ogr <- function(file = "test.geojson") {
    invisible(.Call('sfs_dump2_ogr', PACKAGE = 'sfs', file))
}

